#include "../include/tokenizer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*tokenizer_Try)(Tokenizer *tokenizer, Token *token);

/* @todo this is for debugging, remove later */
void token_print(Token *token)
{
	switch (token->type)
	{
	case TokenType_Ident:
	{
		printf(
			"IDENT \"%.*s\":%d\n",
			token->data.str.len,
			token->data.str.buf,
			token->data.str.len);
	}
	break;
	case TokenType_String:
	{
		printf(
			"STRING \"%.*s\":%d\n",
			token->data.str.len,
			token->data.str.buf,
			token->data.str.len);
	}
	break;
	case TokenType_Integer:
	{
		printf(
			"INTEGER %d\n",
			token->data.integer);
	}
	break;
	case TokenType_Dice:
	{
		printf(
			"DICE %dd%d+%d\n",
			token->data.dice.amount,
			token->data.dice.sides,
			token->data.dice.add);
	}
	break;
	default:
	{
		printf("UNRECOGNIZED TOKEN\n");
		assert(0);
	};
	}
}

Tokenizer tokenizer_make(String8 str, Arena *arena)
{
	Tokenizer tokenizer = {0};
	tokenizer.str = str;
	tokenizer.arena = arena;
	return tokenizer;
}

static bool charIsWhitespace(char c)
{
	return c == ' ' || c == '\t' || c == '\n';
}

static bool charIsDigit(char c)
{
	return '0' <= c && c <= '9';
}

static bool charIsAlpha(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static int tokenizer_skipWhitespace(Tokenizer *tokenizer)
{
	/* skipWhitespace also skips comments */
	char c = 0;
	bool inComment = false;

	for (;;)
	{
		if (tokenizer->index >= tokenizer->str.len)
		{
			return 0;
		}
		c = tokenizer->str.buf[tokenizer->index];
		if (!inComment)
		{
			if (c == '#')
			{
				inComment = true;
			}
			else if (!charIsWhitespace(c))
			{
				return 1;
			}
		}
		else
		{
			if (c == '\n')
			{
				inComment = false;
			}
		}
		tokenizer->index += 1;
	}
}

static int tokenizer_tryParseInteger(Tokenizer *tokenizer, Token *token)
{
	char c = 0;
	uint integer = 0;
	uint len = 0;

	for (;;)
	{
		if (tokenizer->index >= tokenizer->str.len)
		{
			break;
		}
		c = tokenizer->str.buf[tokenizer->index];
		if (charIsDigit(c))
		{
			len++;
			integer = integer * 10 + c - '0';
			tokenizer->index += 1;
		}
		else
		{
			break;
		}
	}

	if (len > 0)
	{
		token->type = TokenType_Integer;
		token->data.integer = integer;

		return 1;
	}
	else
	{
		return 0;
	}
}

static int tokenizer_tryParseDice(Tokenizer *tokenizer, Token *token)
{
	char c = 0;
	uint amount = 0;
	uint sides = 0;
	uint add = 0;
	bool foundOneDigit = false;

	for (;;)
	{
		if (tokenizer->index >= tokenizer->str.len)
		{
			return 0;
		}
		c = tokenizer->str.buf[tokenizer->index];
		if (charIsDigit(c))
		{
			foundOneDigit = true;
			amount = amount * 10 + c - '0';
			tokenizer->index += 1;
		}
		else
		{
			if (foundOneDigit)
			{
				break;
			}
			else
			{
				return 0;
			}
		}
	}

	if (c != 'd')
	{
		return 0;
	}
	tokenizer->index += 1;

	for (;;)
	{
		if (tokenizer->index >= tokenizer->str.len)
		{
			break;
		}
		c = tokenizer->str.buf[tokenizer->index];
		if (charIsDigit(c))
		{
			sides = sides * 10 + c - '0';
			tokenizer->index += 1;
		}
		else
		{
			break;
		}
	}

	if (c == '+')
	{
		tokenizer->index += 1;
		for (;;)
		{
			if (tokenizer->index >= tokenizer->str.len)
			{
				break;
			}
			c = tokenizer->str.buf[tokenizer->index];
			if (charIsDigit(c))
			{
				add = add * 10 + c - '0';
				tokenizer->index += 1;
			}
			else
			{
				break;
			}
		}
	}

	token->type = TokenType_Dice;
	token->data.dice.amount = amount;
	token->data.dice.sides = sides;
	token->data.dice.add = add;

	printf("hi\n");
	token_print(token);

	return 1;
}

static int tokenizer_tryParseIdent(Tokenizer *tokenizer, Token *token)
{
	char c = 0;
	uint len = 0;
	const uint originalIndex = tokenizer->index;

	for (;;)
	{
		if (tokenizer->index >= tokenizer->str.len)
		{
			break;
		}
		c = tokenizer->str.buf[tokenizer->index];
		if (charIsAlpha(c))
		{
			len += 1;
			tokenizer->index += 1;
		}
		else
		{
			break;
		}
	}

	if (len > 0)
	{
		token->type = TokenType_Ident;
		token->data.str.buf = &tokenizer->str.buf[originalIndex];
		token->data.str.len = len;

		return 1;
	}
	else
	{
		return 0;
	}
}

static int tokenizer_tryParseString(Tokenizer *tokenizer, Token *token)
{
	char c = 0;
	bool escape = false;
	bool foundString = false;
	bool isSingleQuotes = false;
	char *buffer = NULL;
	uint bufferLen = 0;

	buffer = arenaPush(tokenizer->arena, 0, 1);

	if (buffer == NULL)
	{
		fprintf(stderr, "out of memory error\n");
		assert(0);
	}

	for (;;)
	{
		if (tokenizer->index >= tokenizer->str.len)
		{
			return 0;
		}

		c = tokenizer->str.buf[tokenizer->index];

		if (!foundString)
		{
			if (c == '"')
			{
				isSingleQuotes = false;
				foundString = true;
			}
			if (c == '\'')
			{
				isSingleQuotes = true;
				foundString = true;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if (escape)
			{
				if (arenaPush(tokenizer->arena, 1, 1) == NULL)
				{
					fprintf(stderr, "out of memory error\n");
					assert(0);
				}
				switch (c)
				{
				case 'n':
				{
					buffer[bufferLen++] = '\n';
				}
				break;
				case '\\':
				{
					buffer[bufferLen++] = '\\';
				}
				break;
				case 't':
				{
					buffer[bufferLen++] = '\t';
				}
				break;
				case '"':
				{
					buffer[bufferLen++] = '"';
				}
				break;
				case '\'':
				{
					buffer[bufferLen++] = '\'';
				}
				break;
				default:
				{
					buffer[bufferLen++] = c;
				}
				break;
				}
			}
			else
			{
				if (!isSingleQuotes && c == '"')
				{
					tokenizer->index += 1;
					break;
				}
				if (isSingleQuotes && c == '\'')
				{
					tokenizer->index += 1;
					break;
				}
				else if (c == '\\')
				{
					escape = true;
				}
				else
				{
					if (arenaPush(tokenizer->arena, 1, 1) == NULL)
					{
						fprintf(stderr, "out of memory error\n");
						assert(0);
					}

					buffer[bufferLen++] = c;
				}
			}
		}

		tokenizer->index += 1;
	}
	if (arenaPush(tokenizer->arena, 1, 1) == NULL)
	{
		fprintf(stderr, "out of memory error\n");
		assert(0);
	}
	buffer[bufferLen] = '\0';

	assert(
		(char *)&tokenizer->arena->memory[tokenizer->arena->top - bufferLen - 1] ==
		buffer);

	token->type = TokenType_String;
	token->data.str.buf = buffer;
	token->data.str.len = bufferLen;

	return 1;
}

int tokenizer_advanceToken(Tokenizer *tokenizer)
{
	assert(tokenizer->hasToken);
	tokenizer->hasToken = false;
	return 1;
}

int tokenizer_popToken(Tokenizer *tokenizer, Token *token)
{
	uint success = 0;
	uint index = 0;
	uint oldTokenizerIndex = 0;
	tokenizer_Try tryFuncs[] = {
		tokenizer_tryParseDice,
		tokenizer_tryParseIdent,
		tokenizer_tryParseString,
		tokenizer_tryParseInteger};

	if (tokenizer->hasToken)
	{
		*token = tokenizer->token;
		tokenizer->hasToken = false;
		return 1;
	}

	success = tokenizer_skipWhitespace(tokenizer);
	if (!success)
	{
		return 0;
	}

	if (tokenizer->index <= tokenizer->str.len)
	{
		for (index = 0; index < sizeof(tryFuncs) / sizeof(*tryFuncs); index++)
		{
			oldTokenizerIndex = tokenizer->index;
			if (tryFuncs[index](tokenizer, token) == 1)
			{
				return 1;
			}
			tokenizer->index = oldTokenizerIndex;
		}
	}

	return 0;
}

int tokenizer_peekToken(Tokenizer *tokenizer, Token *token)
{
	int success = 0;

	if (tokenizer->hasToken)
	{
		*token = tokenizer->token;
		return 1;
	}
	else
	{
		success = tokenizer_popToken(tokenizer, token);
		if (success)
		{
			tokenizer->hasToken = true;
			tokenizer->token = *token;
		}
		return success;
	}
}

bool token_isIdent(Token *token, String8 str)
{
	uint i = 0;

	if (token->type != TokenType_Ident)
	{
		return false;
	}

	if (str.len != token->data.str.len)
	{
		return false;
	}

	for (; i < str.len; i++)
	{
		if (str.buf[i] != token->data.str.buf[i])
		{
			return false;
		}
	}

	return true;
}

bool tokenizer_popIdent(Tokenizer *tokenizer, Token *token)
{
	tokenizer_popToken(tokenizer, token);
	if (token->type != TokenType_Ident)
	{
		return false;
	}
	return true;
}

bool tokenizer_peekIdent(Tokenizer *tokenizer, Token *token)
{
	tokenizer_peekToken(tokenizer, token);
	if (token->type != TokenType_Ident)
	{
		return false;
	}
	return true;
}

bool tokenizer_popInteger(Tokenizer *tokenizer, int *integer)
{
	Token token = {0};
	tokenizer_popToken(tokenizer, &token);
	if (token.type != TokenType_Integer)
	{
		return false;
	}
	*integer = token.data.integer;
	return true;
}

bool tokenizer_expectIdent(Tokenizer *tokenizer, String8 identStr)
{
	Token token = {0};
	tokenizer_peekToken(tokenizer, &token);
	if (token.type == TokenType_Ident && string8Eq(identStr, token.data.str))
	{
		tokenizer_advanceToken(tokenizer);
		return true;
	}
	return false;
}

int tokenizer_getIntegerField(Tokenizer *tokenizer, String8 fieldName, int *integer)
{
	Token token = {0};
	if (!tokenizer_peekIdent(tokenizer, &token))
	{
		return 0;
	}

	if (token_isIdent(&token, fieldName))
	{
		tokenizer_advanceToken(tokenizer);
		assert(tokenizer_popInteger(tokenizer, integer));
		return 1;
	}

	return 0;
}

int tokenizer_getIdentField(Tokenizer *tokenizer, String8 fieldName, String8 *ident)
{
	Token token = {0};
	Token identToken = {0};
	if (!tokenizer_peekIdent(tokenizer, &token))
	{
		return 0;
	}

	if (token_isIdent(&token, fieldName))
	{
		tokenizer_advanceToken(tokenizer);
		assert(tokenizer_popIdent(tokenizer, &identToken));
		*ident = identToken.data.str;

		return 1;
	}

	return 0;
}

bool tokenizer_popString(Tokenizer *tokenizer, String8 *string)
{
	Token token = {0};
	tokenizer_popToken(tokenizer, &token);
	if (token.type != TokenType_String)
	{
		return false;
	}

	string->len = token.data.str.len;
	string->buf = arenaPush(tokenizer->arena, token.data.str.len, 1);
	memcpy(string->buf, token.data.str.buf, token.data.str.len);

	return true;
}

bool tokenizer_getStringField(Tokenizer *tokenizer, String8 fieldName, String8 *str)
{
	Token token = {0};
	if (!tokenizer_peekIdent(tokenizer, &token))
	{
		return false;
	}

	if (token_isIdent(&token, fieldName))
	{
		tokenizer_advanceToken(tokenizer);
		assert(tokenizer_popString(tokenizer, str));

		return true;
	}

	return false;
}

bool tokenizer_popDice(Tokenizer *tokenizer, Dice *dice)
{
	Token token = {0};
	tokenizer_popToken(tokenizer, &token);
	if (token.type != TokenType_Dice)
	{
		return false;
	}
	*dice = token.data.dice;
	return true;
}

int tokenizer_getDiceField(Tokenizer *tokenizer, String8 fieldName, Dice *dice)
{
	Token token = {0};
	if (!tokenizer_peekIdent(tokenizer, &token))
	{
		return 0;
	}

	if (token_isIdent(&token, fieldName))
	{
		tokenizer_advanceToken(tokenizer);
		assert(tokenizer_popDice(tokenizer, dice));
		return 1;
	}

	return 0;
}

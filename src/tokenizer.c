#include "tokenizer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* @todo this is for debugging, remove later */
void tokenPrint(Token *token)
{
	switch (token->type) {
		case TokenType_Ident: {
			printf(
				"IDENT \"%.*s\":%d\n",
				token->data.str.len,
				token->data.str.buf,
				token->data.str.len
			);
		} break;
		case TokenType_String: {
			printf(
				"STRING \"%.*s\":%d\n",
				token->data.str.len,
				token->data.str.buf,
				token->data.str.len
			);
		} break;
		case TokenType_Integer: {
			printf(
				"INTEGER %d\n",
				token->data.integer
			);
		} break;
		default: {
			printf("UNRECOGNIZED TOKEN\n");
			abort();
		};
	}
}

Tokenizer tokenizerMake(String8 str, Arena *arena)
{
	Tokenizer tokenizer = {0};
	tokenizer.str = str;
	tokenizer.arena = arena;
	return tokenizer;
}

bool charIsWhitespace(char c)
{
	return c == ' ' || c == '\t' || c == '\n';
}

bool charIsDigit(char c)
{
	return '0' <= c && c <= '9';
}

bool charIsAlpha(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int tokenizerSkipWhitespace(Tokenizer *tokenizer)
{
	char c = 0;

	for (;;) {
		if (tokenizer->index >= tokenizer->str.len) {
			return 1;
		}
		c = tokenizer->str.buf[tokenizer->index];
		if (charIsWhitespace(c)) {
			tokenizer->index += 1;
		} else {
			tokenizer->index = tokenizer->index;
			return 0;
		}
	}
}

typedef int (*TokenizerTry)(Tokenizer *tokenizer, Token *token);

int tokenizerTryParseInteger(Tokenizer *tokenizer, Token *token)
{
	char c = 0;
	uint integer = 0;
	uint len = 0;

	for (;;) {
		if (tokenizer->index >= tokenizer->str.len) {
			break;
		}
		c = tokenizer->str.buf[tokenizer->index];
		if (charIsDigit(c)) {
			len++;
			integer = integer * 10 + c - '0';
			tokenizer->index += 1;
		} else {
			break;
		}
	}

	if (len > 0) {
		token->type = TokenType_Integer;
		token->data.integer = integer;

		return 1;
	} else {
		return 0;
	}
}

int tokenizerTryParseIdent(Tokenizer *tokenizer, Token *token)
{
	char c = 0;
	uint len = 0;
	const uint originalIndex = tokenizer->index;

	for (;;) {
		if (tokenizer->index >= tokenizer->str.len) {
			break;
		}
		c = tokenizer->str.buf[tokenizer->index];
		if (charIsAlpha(c)) {
			len += 1;
			tokenizer->index += 1;
		} else {
			break;
		}
	}

	if (len > 0) {
		token->type = TokenType_Ident;
		token->data.str.buf = &tokenizer->str.buf[originalIndex];
		token->data.str.len = len;

		return 1;
	} else {
		return 0;
	}
}

int tokenizerTryParseString(Tokenizer *tokenizer, Token *token)
{
	char c = 0;
	bool escape = false;
	bool foundString = false;
	bool isSingleQuotes = false;
	char *buffer = NULL;
	uint bufferLen = 0;

	buffer = arenaPush(tokenizer->arena, 0, 1);

	if (buffer == NULL) {
		fprintf(stderr, "out of memory error\n");
		abort();
	}

	for (;;) {
		if (tokenizer->index >= tokenizer->str.len) {
			return 0;
		}

		c = tokenizer->str.buf[tokenizer->index];

		if (!foundString) {
			if (c == '"') {
				isSingleQuotes = false;
				foundString = true;
			} if (c == '\'') {
				isSingleQuotes = true;
				foundString = true;
			} else {
				return 0;
			}
		} else {
			if (escape) {
				if (arenaPush(tokenizer->arena, 1, 1) == NULL) {
					fprintf(stderr, "out of memory error\n");
					abort();
				}
				switch (c) {
					case 'n': {
						buffer[bufferLen++] = '\n';
					} break;
					case '\\': {
						buffer[bufferLen++] = '\\';
					} break;
					case 't': {
						buffer[bufferLen++] = '\t';
					} break;
					case '"': {
						buffer[bufferLen++] = '"';
					} break;
					case '\'': {
						buffer[bufferLen++] = '\'';
					} break;
					default: {
						buffer[bufferLen++] = c;
					} break;
				}
			} else {
				if (!isSingleQuotes && c == '"') {
					tokenizer->index += 1;
					break;
				} if (isSingleQuotes && c == '\'') {
					tokenizer->index += 1;
					break;
				} else if (c == '\\') {
					escape = true;
				} else {
					if (arenaPush(tokenizer->arena, 1, 1) == NULL) {
						fprintf(stderr, "out of memory error\n");
						abort();
					}

					buffer[bufferLen++] = c;
				}
			}
		}

		tokenizer->index += 1;
	}
	if (arenaPush(tokenizer->arena, 1, 1) == NULL) {
		fprintf(stderr, "out of memory error\n");
		abort();
	}
	buffer[bufferLen] = '\0';

	assert(
		(char *)&tokenizer->arena->memory[tokenizer->arena->top - bufferLen - 1]
		==
		buffer
	);

	token->type = TokenType_String;
	token->data.str.buf = buffer;
	token->data.str.len = bufferLen;

	return 1;
}

int tokenizerAdvanceToken(Tokenizer *tokenizer)
{
	assert(tokenizer->hasToken);
	tokenizer->hasToken = true;
	return 1;
}

int tokenizerPopToken(Tokenizer *tokenizer, Token *token)
{
	uint err = 0;
	uint index = 0;
	uint oldTokenizerIndex = 0;
	TokenizerTry tryFuncs[] = {
		tokenizerTryParseIdent,
		tokenizerTryParseString,
		tokenizerTryParseInteger
	};

	if (tokenizer->hasToken) {
		*token = tokenizer->token;
		tokenizer->hasToken = false;
		return 0;
	}

	err = tokenizerSkipWhitespace(tokenizer);
	if (err) {
		return err;
	}

	for (index = 0; index < sizeof(tryFuncs) / sizeof(*tryFuncs); index++) {
		oldTokenizerIndex = tokenizer->index;
		if (tryFuncs[index](tokenizer, token) == 1) {
			return 0;
		}
		tokenizer->index = oldTokenizerIndex;
	}

	return 1;
}

int tokenizerPeekToken(Tokenizer *tokenizer, Token *token)
{
	int success = 0;

	if (tokenizer->hasToken) {
		*token = tokenizer->token;
		return 1;
	} else {
		success = tokenizerPopToken(tokenizer, token);
		if (success) {
			tokenizer->hasToken = true;
			tokenizer->token = *token;
		}
		return success;
	}
}

bool tokenIsIdent(Token *token, String8 str)
{
	uint i = 0;

	if (token->type != TokenType_Ident) {
		return false;
	}

	if (str.len != token->data.str.len) {
		return false;
	}

	for (; i < str.len; i++) {
		if (str.buf[i] != token->data.str.buf[i]) {
			return false;
		}
	}

	return true;
}

bool tokenizerPopIdent(Tokenizer *tokenizer, Token *token)
{
	tokenizerPopToken(tokenizer, token);
	if (token->type != TokenType_Ident) {
		return false;
	}
	return true;
}

bool tokenizerPeekIdent(Tokenizer *tokenizer, Token *token)
{
	tokenizerPeekToken(tokenizer, token);
	if (token->type != TokenType_Ident) {
		return false;
	}
	return true;
}

bool tokenizerPopInteger(Tokenizer *tokenizer, int *integer) {
	Token token = {0};
	tokenizerPopToken(tokenizer, &token);
	if (token.type != TokenType_Ident) {
		return false;
	}
	*integer = token.data.integer;
	return true;
}

int tokenizer_getIntegerField(Tokenizer *tokenizer, String8 fieldName, int *integer)
{
	Token token = {0};
	if (!tokenizerPeekIdent(tokenizer, &token)) {
		return 0;
	}

	if (tokenIsIdent(&token, fieldName)) {
		tokenizerAdvanceToken(tokenizer);
		if (!tokenizerPopInteger(tokenizer, integer)) {
			abort();
		}
		return 1;
	}

	return 0;
}

int tokenizer_getIdentField(Tokenizer *tokenizer, String8 fieldName, String8 *ident)
{
	Token token = {0};
	Token identToken = {0};
	if (!tokenizerPeekIdent(tokenizer, &token)) {
		return 0;
	}

	if (tokenIsIdent(&token, fieldName)) {
		tokenizerAdvanceToken(tokenizer);
		if (!tokenizerPopIdent(tokenizer, &identToken)) {
			abort();
		}
		*ident = identToken.data.str;

		return 1;
	}

	return 0;
}


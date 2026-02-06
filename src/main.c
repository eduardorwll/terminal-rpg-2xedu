#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define S8 string8FromCstr

typedef unsigned int uint;

typedef struct String8 String8;
typedef struct Tokenizer Tokenizer;
typedef struct Token Token;
typedef struct ItemType ItemType;
typedef struct Item Item;
typedef struct MonsterType MonsterType;
typedef struct Monster Monster;
typedef struct GameData GameData;
typedef struct Game Game;

typedef enum 
{
	TokenType_Ident,
	TokenType_String,
	TokenType_Integer,
	TokenType_Dice
} TokenType;

struct String8
{
	uint len;
	char *buf;
};

struct Token
{
	TokenType type;
	union {
		int integer;
		struct {
			int amount;
			int sides;
		} dice;
		String8 str;
	} data;
};

struct Tokenizer
{
	String8 str;
	uint index;

	bool hasToken;
	Token token;
};

struct ItemType
{
	String8 name;
	ItemType *next;
};

struct Item
{
	ItemType type;
};

struct MonsterType
{
	String8 name;
	int maxHp;

	MonsterType *next;
};

struct Monster
{
	MonsterType *type;
};

struct GameData
{
	MonsterType *monsterTypeHead;
	ItemType *itemTypeHead;
};

String8 string8FromCstr(char *str) {
	String8 s8 = {0};
	s8.len = strlen(str);
	s8.buf = str;
	return s8;
}

int readWholeFile(char *filepath, String8 *str);
int readWholeFile(char *filepath, String8 *str);
int writeToFile(String8 str, char *filepath);
void tokenPrint(Token *token);
Tokenizer tokenizerMake(String8 str);
bool charIsWhitespace(char c);
bool charIsDigit(char c);
bool charIsAlpha(char c);
int tokenizerSkipWhitespace(Tokenizer *tokenizer);
int tokenizerTryParseInteger(Tokenizer *tokenizer, Token *token);
int tokenizerTryParseIdent(Tokenizer *tokenizer, Token *token);
int tokenizerTryParseString(Tokenizer *tokenizer, Token *token);
int tokenizerPopToken(Tokenizer *tokenizer, Token *token);
bool tokenIsIdent(Token *token, String8 str);
bool tokenizerPopIdent(Tokenizer *tokenizer, Token *token);
bool tokenizerPopInteger(Tokenizer *tokenizer, int *integer);
int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt);

int readWholeFile(char *filepath, String8 *str)
{
	FILE *fp = fopen(filepath, "r");

	if (fp == NULL) {
		return 1;
	}

	if (fseek(fp, 0L, SEEK_END) != 0) {
		fclose(fp);
		return 1;
	}
	str->len = ftell(fp);
	rewind(fp);

	str->buf = malloc(str->len);
	if (str->buf == NULL) {
		fclose(fp);
		return 1;
	}

	if (fread(str->buf, 1, str->len, fp) != str->len) {
		free(str->buf);
		fclose(fp);
		return 1;
	}

	fclose(fp);

	return 0;
}

int writeToFile(String8 str, char *filepath)
{
	FILE *fp = fopen(filepath, "w");
	if (fp == NULL) {
		return 1;
	}

	if (fwrite(str.buf, 1, str.len, fp) != str.len) {
		return 1;
	}

	fclose(fp);

	return 0;
}

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

Tokenizer tokenizerMake(String8 str)
{
	Tokenizer tokenizer = {0};
	tokenizer.str = str;
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
	uint bufferCapacity = 32;
	uint bufferLen = 0;
	buffer = malloc(bufferCapacity);
	if (buffer == NULL) {
		fprintf(stderr, "malloc error\n");
		abort();
	}

	for (;;) {
		if (tokenizer->index >= tokenizer->str.len) {
			return 0;
		}

		c = tokenizer->str.buf[tokenizer->index];

		if (bufferLen + 1 >= bufferCapacity) {
			bufferCapacity *= 2;
			buffer = realloc(buffer, bufferCapacity);
			if (buffer == NULL) {
				fprintf(stderr, "malloc error\n");
				abort();
			}
		}

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
					buffer[bufferLen++] = c;
				}
			}
		}

		tokenizer->index += 1;
	}

	token->type = TokenType_String;
	token->data.str.buf = malloc(bufferLen);
	if (token->data.str.buf == NULL) {
		fprintf(stderr, "malloc error\n");
		abort();
	}
	token->data.str.len = bufferLen;
	memcpy(token->data.str.buf, buffer, bufferLen);
	free(buffer);

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

bool tokenIsIdent(Token *token, String8 str) {
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

bool tokenizerPopIdent(Tokenizer *tokenizer, Token *token) {
	tokenizerPopToken(tokenizer, token);
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

int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt)
{
	Token token = {0};
	int integer = 0;

	if (!tokenizerPopIdent(tokenizer, &token)) {
		return 0;
	}

	if (!tokenIsIdent(&token, S8("MONSTER"))) {
		return 0;
	}

	for (;;) {
		if (!tokenizerPopIdent(tokenizer, &token)) {
			return 1;
		}

		if (tokenIsIdent(&token, S8("maxhp"))) {
			if (tokenizerPopInteger(tokenizer, &integer)) {abort();}
			mt->maxHp = integer;
		} else {
			return 1;
		}
	}

	return 1;
}

int main()
{
	uint i = 0;
	Tokenizer tokenizer = tokenizerMake(S8("these are 3 some 'lol string' tokens"));
	Token token = {0};

	while (tokenizerPopToken(&tokenizer, &token) == 0) {
		tokenPrint(&token);
		i++;
		if (i > 30) {abort();}
	}

    return 0;
}

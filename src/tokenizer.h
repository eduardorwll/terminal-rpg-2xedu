#if !defined(TOKENIZER_H)
#define TOKENIZER_H

#include "arena.h"
#include "string8.h"
#include "rng.h"

typedef struct Tokenizer Tokenizer;
typedef struct Token Token;

typedef enum
{
	TokenType_Ident,
	TokenType_String,
	TokenType_Integer,
	TokenType_Dice
} TokenType;

struct Token
{
	TokenType type;
	union
	{
		int integer;
		String8 str;
		Dice dice;
	} data;
};

struct Tokenizer
{
	String8 str;
	uint index;

	bool hasToken;
	Token token;

	Arena *arena;
};

void token_print(Token *token);
Tokenizer tokenizer_make(String8 str, Arena *arena);

int tokenizer_advanceToken(Tokenizer *tokenizer);
int tokenizer_popToken(Tokenizer *tokenizer, Token *token);
int tokenizer_peekToken(Tokenizer *tokenizer, Token *token);

bool token_isIdent(Token *token, String8 str);

bool tokenizer_popIdent(Tokenizer *tokenizer, Token *token);
bool tokenizer_peekIdent(Tokenizer *tokenizer, Token *token);
int tokenizer_getIdentField(Tokenizer *tokenizer, String8 fieldName, String8 *ident);
bool tokenizer_expectIdent(Tokenizer *tokenizer, String8 identStr);

bool tokenizer_popInteger(Tokenizer *tokenizer, int *integer);
int tokenizer_getIntegerField(Tokenizer *tokenizer, String8 fieldName, int *integer);

bool tokenizer_popString(Tokenizer *tokenizer, String8 *str);
bool tokenizer_getStringField(Tokenizer *tokenizer, String8 fieldname, String8 *str);

bool tokenizer_popDice(Tokenizer *tokenizer, Dice *dice);
int tokenizer_getDiceField(Tokenizer *tokenizer, String8 fieldName, Dice *dice);

#endif /* !defined(TOKENIZER_H) */

#if !defined(TOKENIZER_H)
#define TOKENIZER_H

#include "arena.h"
#include "string8.h"

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

	Arena *arena;
};

void tokenPrint(Token *token);
Tokenizer tokenizerMake(String8 str, Arena *arena);
/*bool charIsWhitespace(char c);*/
/*bool charIsDigit(char c);*/
/*bool charIsAlpha(char c);*/
/*int tokenizerSkipWhitespace(Tokenizer *tokenizer);*/
/*int tokenizerTryParseInteger(Tokenizer *tokenizer, Token *token);*/
/*int tokenizerTryParseIdent(Tokenizer *tokenizer, Token *token);*/
/*int tokenizerTryParseString(Tokenizer *tokenizer, Token *token);*/
int tokenizerAdvanceToken(Tokenizer *tokenizer);
int tokenizerPopToken(Tokenizer *tokenizer, Token *token);
int tokenizerPeekToken(Tokenizer *tokenizer, Token *token);
bool tokenIsIdent(Token *token, String8 str);
bool tokenizerPopIdent(Tokenizer *tokenizer, Token *token);
bool tokenizerPeekIdent(Tokenizer *tokenizer, Token *token);
bool tokenizerPopInteger(Tokenizer *tokenizer, int *integer);
int tokenizer_getIntegerField(Tokenizer *tokenizer, String8 fieldName, int *integer);
int tokenizer_getIdentField(Tokenizer *tokenizer, String8 fieldName, String8 *ident);

#endif /* !defined(TOKENIZER_H) */

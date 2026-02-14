#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "string8.h"
#include "arena.h"
#include "tokenizer.h"
#include "util.h"

typedef struct ItemType ItemType;
typedef struct Item Item;
typedef struct MonsterType MonsterType;
typedef struct Monster Monster;
typedef struct GameData GameData;
typedef struct Game Game;

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
	int atk;

	uint isUndead : 1;

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

int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt);

int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt)
{
	Token token = {0};
	String8 skillStr = {0};

	if (!tokenizerPeekIdent(tokenizer, &token)) {
		return 0;
	}

	if (!tokenIsIdent(&token, S8("MONSTER"))) {
		return 0;
	}
	tokenizerAdvanceToken(tokenizer);

	for (;;) {
		if (tokenizer_getIntegerField(tokenizer, S8("hp"), &mt->maxHp)) {}
		if (tokenizer_getIntegerField(tokenizer, S8("atk"), &mt->atk)) {}
		if (tokenizer_getIdentField(tokenizer, S8("skill"), &skillStr)) {
			if (string8Eq(skillStr, S8("undead"))) {
				mt->isUndead = true;
			}
		} else {
			break;
		}
	}

	return 1;
}

int main()
{
	uint i = 0;
	Arena arena = {0};
	Tokenizer tokenizer = {0};
	Token token = {0};

	arena.capacity = 1024 * 4;  /* 4kb should be enough for everyone ;) */
	arena.memory = malloc(arena.capacity);

	tokenizer = tokenizerMake(S8("these are 3 some 'lol string' tokens"), &arena);

	while (tokenizerPopToken(&tokenizer, &token) == 0) {
		tokenPrint(&token);
		i++;
		if (i > 30) {abort();}
	}

    return 0;
}

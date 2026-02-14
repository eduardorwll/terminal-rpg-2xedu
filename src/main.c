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
typedef struct Dice Dice;

struct Dice
{
	int add;
	int amount;
	int sides;
};

struct ItemType
{
	String8 name;

	bool doesRegen;
	bool doesPoison;

	int hp;
	int atk;
};

struct Item
{
	ItemType type;
	Item *next;
};

struct MonsterType
{
	String8 name;
	int maxHp;
	int atk;

	uint isUndead : 1;
};

struct Monster
{
	MonsterType *type;
	int hp;
	int regenTimer;
	int poisonTimer;

	Item *weapon;
	Item *armor;
	Item *inventory;

	uint undeadResurrectTimer;
};

struct GameData
{
	MonsterType *monsterTypes;
	uint monsterTypesLen;

	ItemType *itemTypes;
	uint itemTypesLen;
};

#define MAX_ENEMIES 6

typedef enum
{
	GameMode_Menu,
	GameMode_Battle
} GameMode;

struct Game
{
	GameData gameData;
	GameMode mode;

	Monster player;

	/* battle data */
	Monster enemies[MAX_ENEMIES];
	uint enemiesLen;
};

int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt);
int tryParseItemType(Tokenizer *tokenizer, ItemType *it);

int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt)
{
	Token token = {0};
	String8 str = {0};

	if (!tokenizer_peekIdent(tokenizer, &token)) {
		return 0;
	}

	if (!token_isIdent(&token, S8("MONSTER"))) {
		return 0;
	}
	tokenizer_advanceToken(tokenizer);

	for (;;) {
		if      (tokenizer_getIntegerField(tokenizer, S8("hp"), &mt->maxHp)) {}
		else if (tokenizer_getIntegerField(tokenizer, S8("atk"), &mt->atk)) {}
		else if (tokenizer_getIdentField(tokenizer, S8("skill"), &str)) {
			if (string8Eq(str, S8("undead"))) {
				mt->isUndead = true;
			}
		} else if (tokenizer_getStringField(tokenizer, S8("name"), &str)) {
			/* name */
		} else {
			break;
		}
	}

	return 1;
}

int tryParseItemType(Tokenizer *tokenizer, ItemType *it)
{
	Token token = {0};

	if (!tokenizer_peekIdent(tokenizer, &token)) {
		return 0;
	}

	if (!token_isIdent(&token, S8("ITEM"))) {
		return 0;
	}
	tokenizer_advanceToken(tokenizer);

	for (;;) {
		if      (tokenizer_getIntegerField(tokenizer, S8("hp"),  &it->hp)) {}
		else if (tokenizer_getIntegerField(tokenizer, S8("atk"), &it->atk)) {}
		else if (tokenizer_expectIdent(tokenizer, S8("regen"))) {
			it->doesRegen = true;
		} else if (tokenizer_expectIdent(tokenizer, S8("poison"))) {
			it->doesPoison = true;
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
	String8 gameDataStr = {0};

	arena.capacity = 1024 * 4; /* 4kb should be enough for everyone ;) */
	arena.memory = malloc(arena.capacity);

	if (readWholeFile("src/gamedata", &gameDataStr)) {
		printf("error while reading src/gamedata\n");
		abort();
	}

	tokenizer = tokenizer_make(gameDataStr, &arena);

	while (tokenizer_popToken(&tokenizer, &token) == 0) {
		token_print(&token);
		/* if (i > 30) {abort();} */
	}

    return 0;
}

#if !defined(GAME_H)
#define GAME_H

#include "string8.h"
#include "arena.h"
#include "tokenizer.h"
#include "util.h"

#define MAX_ENEMIES 6

typedef struct ItemType ItemType;
typedef struct MonsterType MonsterType;
typedef struct GameData GameData;

/* live data */
typedef struct Item Item;
typedef struct Monster Monster;
typedef struct Game Game;

typedef enum
{
	GameMode_Menu,
	GameMode_Battle
} GameMode;

struct ItemType
{
	String8 name;

	bool doesRegen;
	bool doesPoison;

	int hp;
	int atk;

	ItemType *next;
};

struct MonsterType
{
	String8 name;
	int maxHp;
	int atk;

	uint isUndead : 1;
	MonsterType *next;
};

struct GameData
{
	MonsterType *monsterTypesHead;
	ItemType *itemTypesHead;
};

struct Item
{
	ItemType type;
	Item *next;
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

struct Game
{
	GameData gamedata;
	GameMode mode;
	Arena arena; /* try to use this instead of malloc */

	Monster player;

	/* battle data */
	Monster enemies[MAX_ENEMIES];
	uint enemiesLen;
};

int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt);
int tryParseItemType(Tokenizer *tokenizer, ItemType *it);
void parseGameData(Game *game);
void printGameData(GameData *gamedata);

#endif /* !defined(GAME_H) */

#if !defined(GAME_H)
#define GAME_H

#include "string8.h"
#include "arena.h"
#include "tokenizer.h"
#include "util.h"

#define MAX_ENEMIES 6

typedef struct ItemType ItemType;
typedef struct MonsterType MonsterType;
typedef struct Place Place;
typedef struct PlaceEntry PlaceEntry;
typedef struct GameData GameData;

/* live data */
typedef struct Item Item;
typedef struct Monster Monster;
typedef struct Game Game;

typedef enum
{
	PlaceEntryType_Gold,
	PlaceEntryType_Monster,
	PlaceEntryType_Item
} PlaceEntryType;

typedef enum
{
	GameMode_Menu,
	GameMode_Battle
} GameMode;

struct ItemType
{
	String8 name;

	bool isConsumable;
	bool isWeapon;
	bool isArmor;

	bool doesRegen;
	bool doesPoison;

	Dice hp;
	Dice damage;
	int defense;

	ItemType *next;
};

struct MonsterType
{
	String8 name;
	int str;
	int dex;
	int vit;
	int res;

	uint isUndead : 1;
	MonsterType *next;
};

struct PlaceEntry
{
	int chance;

	PlaceEntryType type;
	union {
		ItemType *itemType;
		MonsterType *monsterType;
		int gold;
	} data;

	PlaceEntry *next;
};

struct Place
{
	String8 name;
	int chanceTotal;

	PlaceEntry *entries;

	Place *next;
};

struct GameData
{
	MonsterType *monsterTypesHead;
	ItemType *itemTypesHead;
	Monster *templates;
	Place *places;
};

struct Item
{
	ItemType *type;
	Item *next;
};

struct Monster
{
	MonsterType *type;
	int level;
	int hp;
	int regenTimer;
	int poisonTimer;
	int gold;

	Item weapon;
	Item armor;
	Item *inventory;

	uint undeadResurrectTimer;

	Monster *next;
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

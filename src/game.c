#include "game.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt)
{
	Token token = {0};
	String8 str = {0};

	for (;;) {
		if (!tokenizer_peekToken(tokenizer, &token)) {
			break;
		}

		if (tokenizer_getIntegerField(tokenizer, S8("hp"), &mt->maxHp)) {}
		else if (tokenizer_getIntegerField(tokenizer, S8("atk"), &mt->atk)) {}
		else if (tokenizer_getIdentField(tokenizer, S8("skill"), &str)) {
			if (string8Eq(str, S8("undead"))) {
				mt->isUndead = true;
			}
		} else if (tokenizer_getStringField(tokenizer, S8("name"), &mt->name)) {}
		else {
			break;
		}
	}

	return 1;
}

int tryParseItemType(Tokenizer *tokenizer, ItemType *it)
{
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

void parseGameData(Game *game)
{
	GameData *gamedata = &game->gamedata;
	Tokenizer tokenizer = {0};
	String8 gamedataStr = {0};
	Token token = {0};
	int success = 0;
	MonsterType *monsterType = NULL;
	ItemType *itemType       = NULL;

	if (readWholeFile("src/gamedata", &gamedataStr)) {
		printf("error while reading src/gamedata\n");
		assert(0);
	}

	tokenizer = tokenizer_make(gamedataStr, &game->arena);

	for (;;) {
		if (!tokenizer_popToken(&tokenizer, &token)) {
			break;
		}

		if (token_isIdent(&token, S8("MONSTER"))) {
			monsterType = gamedata->monsterTypesHead;
			gamedata->monsterTypesHead = arenaPush(
				&game->arena,
				sizeof(MonsterType),
				8
			);
			success = tryParseMonsterType(
				&tokenizer,
				gamedata->monsterTypesHead
			);
			gamedata->monsterTypesHead->next = monsterType;
		} else if (token_isIdent(&token, S8("ITEM"))) {
			itemType = gamedata->itemTypesHead;
			gamedata->itemTypesHead = arenaPush(
				&game->arena,
				sizeof(ItemType),
				8
			);
			success = tryParseItemType(
				&tokenizer,
				gamedata->itemTypesHead
			);
			gamedata->itemTypesHead->next = itemType;
		} else {
			printf("unrecognized token: "); 
			token_print(&token);
			printf("\n");
			assert(0);
		}

		assert(success);
	}

	free(gamedataStr.buf);
	gamedataStr.buf = NULL;
}

void printGameData(GameData *gamedata)
{
	MonsterType *monsterType = gamedata->monsterTypesHead;
	while (monsterType != NULL) {
		printf("MONSTER\n");
		printf(
			"    name: %.*s\n",
			monsterType->name.len,
			monsterType->name.buf
		);
		printf(
			"    maxhp: %d\n",
			monsterType->maxHp);
		printf(
			"    atk: %d\n",
			monsterType->atk
		);
		printf(
			"    isUndead: %s\n",
			monsterType->isUndead ? "true" : "false"
		);
		monsterType = monsterType->next;
	}
}

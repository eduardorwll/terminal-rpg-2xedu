#include "../include/game.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt)
{
	Token token = {0};
	String8 str = {0};

	for (;;)
	{
		if (!tokenizer_peekToken(tokenizer, &token))
		{
			break;
		}

		if (tokenizer_getIntegerField(tokenizer, S8("str"), &mt->str))
		{
		}
		else if (tokenizer_getIntegerField(tokenizer, S8("dex"), &mt->dex))
		{
		}
		else if (tokenizer_getIntegerField(tokenizer, S8("vit"), &mt->vit))
		{
		}
		else if (tokenizer_getIntegerField(tokenizer, S8("res"), &mt->res))
		{
		}
		else if (tokenizer_getIdentField(tokenizer, S8("skill"), &str))
		{
			if (string8Eq(str, S8("undead")))
			{
				mt->isUndead = true;
			}
		}
		else if (tokenizer_getStringField(tokenizer, S8("name"), &mt->name))
		{
		}
		else
		{
			break;
		}
	}

	return 1;
}

int tryParseItemType(Tokenizer *tokenizer, ItemType *it)
{
	for (;;)
	{
		if (tokenizer_getIntegerField(tokenizer, S8("defense"), &it->defense))
		{
			it->isArmor = true;
		}
		else if (tokenizer_getDiceField(tokenizer, S8("damage"), &it->damage))
		{
			it->isWeapon = true;
		}
		else if (tokenizer_getDiceField(tokenizer, S8("hp"), &it->hp))
		{
			it->isConsumable = true;
		}
		else if (tokenizer_expectIdent(tokenizer, S8("regen")))
		{
			it->isConsumable = true;
			it->doesRegen = true;
		}
		else if (tokenizer_expectIdent(tokenizer, S8("poison")))
		{
			it->isConsumable = true;
			it->doesPoison = true;
		}
		else if (tokenizer_getStringField(tokenizer, S8("name"), &it->name))
		{
		}
		else
		{
			break;
		}
	}

	return 1;
}

MonsterType *findMonsterType(GameData *gamedata, String8 name)
{
	MonsterType *monsterType = monsterType = gamedata->monsterTypesHead;

	for (;;)
	{
		assert(monsterType != NULL);
		if (string8Eq(monsterType->name, name))
		{
			return monsterType;
		}
		monsterType = monsterType->next;
	}
}

ItemType *findItemType(GameData *gamedata, String8 name)
{
	ItemType *itemType = gamedata->itemTypesHead;

	for (;;)
	{
		assert(itemType != NULL);
		if (string8Eq(itemType->name, name))
		{
			return itemType;
		}
		itemType = itemType->next;
	}
}

void addItemTypeToInventory(Monster *monster, ItemType *itemType)
{
	Item *item = malloc(sizeof(Item));
	assert(item != NULL);
	item->type = itemType;
	item->next = monster->inventory;
	monster->inventory = item;
}

void removeItemFromInventoryIndex(Monster *monster, int index)
{
	Item *prev = NULL;
	Item *curr = monster->inventory;
	while (index > 0)
	{
		assert(curr != NULL);
		if (index == 0)
		{
			if (prev != NULL)
			{
				prev->next = curr->next;
			}
			free(curr);
		}

		prev = curr;
		curr = curr->next;
		index--;
	}
}

/* @todo remember to free items from templates */
int tryParseTemplate(Tokenizer *tokenizer, GameData *gamedata, Monster *template)
{
	String8 str = {0};
	MonsterType *monsterType = NULL;
	ItemType *itemType = NULL;

	for (;;)
	{
		if (tokenizer_getIntegerField(tokenizer, S8("level"), &template->level))
		{
		}
		else if (tokenizer_getIntegerField(tokenizer, S8("gold"), &template->gold))
		{
		}
		else if (tokenizer_getStringField(tokenizer, S8("monster"), &str))
		{
			monsterType = findMonsterType(gamedata, str);
			template->type = monsterType;
		}
		else if (tokenizer_getStringField(tokenizer, S8("item"), &str))
		{
			itemType = findItemType(gamedata, str);
			addItemTypeToInventory(template, itemType);
		}
		else if (tokenizer_getStringField(tokenizer, S8("weapon"), &str))
		{
			itemType = gamedata->itemTypesHead;
			for (;;)
			{
				assert(itemType != NULL);
				if (string8Eq(itemType->name, str))
				{
					template->weapon.type = itemType;
					break;
				}
				itemType = itemType->next;
			}
		}
		else if (tokenizer_getStringField(tokenizer, S8("armor"), &str))
		{
			itemType = gamedata->itemTypesHead;
			for (;;)
			{
				assert(itemType != NULL);
				if (string8Eq(itemType->name, str))
				{
					template->armor.type = itemType;
					break;
				}
				itemType = itemType->next;
			}
		}
		else
		{
			break;
		}
	}

	return 1;
}

int tryParsePlace(Tokenizer *tokenizer, Game *game, GameData *gamedata, Place *place)
{
	PlaceEntry *entry = NULL;
	String8 name = {0};

	for (;;)
	{
		if (tokenizer_expectIdent(tokenizer, S8("gold")))
		{
			entry = arenaPush(&game->arena, sizeof(PlaceEntry), 1);
			assert(entry != NULL);

			entry->type = PlaceEntryType_Gold;
			assert(tokenizer_popInteger(tokenizer, &entry->data.gold));
			assert(tokenizer_popInteger(tokenizer, &entry->chance));

			entry->next = place->entries;
			place->entries = entry;
		}
		else if (tokenizer_expectIdent(tokenizer, S8("enemy")))
		{
			entry = arenaPush(&game->arena, sizeof(PlaceEntry), 1);
			assert(entry != NULL);

			entry->type = PlaceEntryType_Monster;
			assert(tokenizer_popString(tokenizer, &name));
			assert(tokenizer_popInteger(tokenizer, &entry->chance));

			entry->data.monsterType = findMonsterType(gamedata, name);

			entry->next = place->entries;
			place->entries = entry;
		}
		else if (tokenizer_expectIdent(tokenizer, S8("item")))
		{
			entry = arenaPush(&game->arena, sizeof(PlaceEntry), 1);
			assert(entry != NULL);

			entry->type = PlaceEntryType_Item;
			assert(tokenizer_popString(tokenizer, &name));
			assert(tokenizer_popInteger(tokenizer, &entry->chance));

			entry->data.itemType = findItemType(gamedata, name);

			entry->next = place->entries;
			place->entries = entry;
		}
		else if (tokenizer_getStringField(tokenizer, S8("name"), &place->name))
		{
		}
		else
		{
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
	ItemType *itemType = NULL;
	Monster *template = NULL;
	Place *place = NULL;

	if (readWholeFile("src/gamedata", &gamedataStr))
	{
		printf("error while reading src/gamedata\n");
		assert(0);
	}

	tokenizer = tokenizer_make(gamedataStr, &game->arena);

	for (;;)
	{
		if (!tokenizer_popToken(&tokenizer, &token))
		{
			break;
		}

		if (token_isIdent(&token, S8("MONSTER")))
		{
			monsterType = gamedata->monsterTypesHead;
			gamedata->monsterTypesHead = arenaPush(
				&game->arena,
				sizeof(MonsterType),
				8);
			success = tryParseMonsterType(
				&tokenizer,
				gamedata->monsterTypesHead);
			gamedata->monsterTypesHead->next = monsterType;
		}
		else if (token_isIdent(&token, S8("ITEM")))
		{
			itemType = gamedata->itemTypesHead;
			gamedata->itemTypesHead = arenaPush(
				&game->arena,
				sizeof(ItemType),
				8);
			success = tryParseItemType(
				&tokenizer,
				gamedata->itemTypesHead);
			gamedata->itemTypesHead->next = itemType;
		}
		else if (token_isIdent(&token, S8("TEMPLATE")))
		{
			template = gamedata->templates;
			gamedata->templates = arenaPush(
				&game->arena,
				sizeof(Monster),
				8);
			success = tryParseTemplate(
				&tokenizer,
				gamedata,
				gamedata->templates);
			gamedata->templates->next = template;
		}
		else if (token_isIdent(&token, S8("PLACE")))
		{
			place = gamedata->places;
			gamedata->places = arenaPush(
				&game->arena,
				sizeof(Place),
				8);
			success = tryParsePlace(
				&tokenizer,
				game,
				gamedata,
				gamedata->places);
			gamedata->places->next = place;
		}
		else
		{
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
	ItemType *itemType = gamedata->itemTypesHead;

	while (monsterType != NULL)
	{
		printf("MONSTER\n");
		printf(
			"    name: %.*s\n",
			monsterType->name.len,
			monsterType->name.buf);
		printf("    str: %d\n", monsterType->str);
		printf("    dex: %d\n", monsterType->dex);
		printf("    vit: %d\n", monsterType->vit);
		printf("    res: %d\n", monsterType->res);
		printf(
			"    isUndead: %s\n",
			monsterType->isUndead ? "true" : "false");
		monsterType = monsterType->next;
	}

	while (itemType != NULL)
	{
		printf("ITEM\n");
		printf(
			"    name: %.*s\n",
			itemType->name.len,
			itemType->name.buf);
		if (itemType->isWeapon)
		{
			printf(
				"    damage: %dd%d+%d\n",
				itemType->damage.amount,
				itemType->damage.sides,
				itemType->damage.add);
		}
		if (itemType->isArmor)
		{
			printf("    defense: %d\n", itemType->defense);
		}
		if (itemType->isConsumable)
		{
			printf("    doesRegen: %s\n", itemType->doesRegen ? "true" : "false");
			printf("    doesPoison: %s\n", itemType->doesPoison ? "true" : "false");
			printf(
				"    hp: %dd%d+%d\n",
				itemType->hp.amount,
				itemType->hp.sides,
				itemType->hp.add);
		}
		itemType = itemType->next;
	}
}

int getInputNumber(int lo, int hi)
{
	int n = 0;
	for (;;)
	{
		printf("> ");
		scanf("%d", &n);
		if (lo <= n && n <= hi)
		{
			return n;
		}
		else
		{
			printf("A opcao precisa ser entre %d e %d\n", lo, hi);
		}
	}
}

void smallDelay()
{
	usleep(200000);
}

void largeDelay()
{
	usleep(1200000);
}

void removeDeadEnemies(Game *game)
{
	uint i = 0, j = 0;
	for (i = 0; i < game->enemiesLen; i++)
	{
		while (game->enemies[i].hp <= 0)
		{
			for (j = 0; j < game->enemiesLen - 1; j++)
			{
				game->enemies[j] = game->enemies[j + 1];
			}
			game->enemiesLen -= 1;
		}
	}
}

void gameBattle(Game *game)
{
	int option = 0;
	int damage = 0;
	bool getAttacked = true;
	uint monsterIndex = 0;

	Monster *enemy = NULL;
	MonsterType *monsterType = NULL;

	uint itemIndex = 0;
	Item *item = NULL;

	printf("O que deseja fazer?\n");
	printf("1. Atacar monstro\n");
	printf("2. Usar item\n");
	printf("3. Fugir\n");
	printf("4. Ver monstros\n");
	option = getInputNumber(1, 4);
	switch (option)
	{
		case 1:
		{
			printf("Atacar qual monstro?\n");
			for (monsterIndex = 0; monsterIndex < game->enemiesLen; monsterIndex++)
			{
				enemy = &game->enemies[monsterIndex];
				monsterType = enemy->type;
				printf(
					"[%d] %.*s\n",
					monsterIndex + 1,
					monsterType->name.len,
					monsterType->name.buf
				);
				smallDelay();
			}
			option = getInputNumber(1, game->enemiesLen + 1) - 1;
			printf("option = %d\n", option);

			enemy = &game->enemies[option];

			monsterType = game->player.type;
			damage = monsterType->str * 2;

			smallDelay();
			printf("o heroi ataca!\n");
			smallDelay();
			printf("o monstro perde %d de hp!\n", damage);
			enemy->hp -= damage;
			smallDelay();
			printf("o monstro agora tem %d hp!\n\n", enemy->hp);
			smallDelay();
		}
		break;
		case 2:
		{
			printf("inventario:\n");
			item = game->player.inventory;
			itemIndex = 0;

			while (item != NULL)
			{
				monsterType = enemy->type;
				printf(
					"[%d] %.*s\n",
					itemIndex + 1,
					item->type->name.len,
					item->type->name.buf
				);
				item = item->next;
				itemIndex += 1;
				smallDelay();
			}
		}
		break;
		case 3:
		{
			assert(0);
		}
		break;
		case 4:
		{
			for (monsterIndex = 0; monsterIndex < game->enemiesLen; monsterIndex++)
			{
				enemy = &game->enemies[monsterIndex];
				monsterType = enemy->type;
				printf(
					"[%d] %.*s\n",
					monsterIndex + 1,
					monsterType->name.len,
					monsterType->name.buf
				);
				smallDelay();
			}
			printf("\n");
			getAttacked = false;
			smallDelay();
		}
		break;
	}

	/* monster's turn */
	if (getAttacked)
	{
		largeDelay();
		for (monsterIndex = 0; monsterIndex < game->enemiesLen; monsterIndex++)
		{
			enemy = &game->enemies[monsterIndex];
			if (enemy->hp <= 0)
			{
				continue;
			}

			monsterType = enemy->type;
			damage = monsterType->str * 2;
			printf("o monstro '%.*s' ataca!\n", monsterType->name.len, monsterType->name.buf);
			smallDelay();
			printf("o heroi perde %d de hp!\n\n", damage);
			game->player.hp -= damage;
			smallDelay();
		}
	}
	removeDeadEnemies(game);
}

void gameWalk(Game *game)
{
	(void) game;
}

int gameUpdate(Game *game)
{
	game->place = &game->gamedata.places[0];
	if (game->enemiesLen > 0) {
		gameBattle(game);
	} else {
		gameWalk(game);
	}
	return 0;
}

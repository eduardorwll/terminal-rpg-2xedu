#include "../include/game.h"

#include "../include/rng.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int tryParseMonsterType(Tokenizer *tokenizer, MonsterType *mt)
{
	Token token = {0};

	for (;;)
	{
		if (!tokenizer_peekToken(tokenizer, &token))
		{
			break;
		}

		if (tokenizer_getIntegerField(tokenizer, S8("str"), &mt->str))
		{
		}
		else if (tokenizer_getIntegerField(tokenizer, S8("vit"), &mt->vit))
		{
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
		if (tokenizer_getDiceField(tokenizer, S8("hp"), &it->hp))
		{
		}
		else if (tokenizer_expectIdent(tokenizer, S8("regen")))
		{
			it->doesRegen = true;
		}
		else if (tokenizer_expectIdent(tokenizer, S8("poison")))
		{
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

MonsterType *getMonsterType(GameData *gamedata, int index)
{
	int count = 0;
	MonsterType *monsterType = monsterType = gamedata->monsterTypesHead;

	while (count != index)
	{
		monsterType = monsterType->next;
		count++;
	}

	return monsterType;
}

int monsterTypeCount(GameData *gamedata)
{
	int count = 0;
	MonsterType *monsterType = monsterType = gamedata->monsterTypesHead;

	while (monsterType != NULL)
	{
		monsterType = monsterType->next;
		count++;
	}

	return count;
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

void parseGameData(Game *game)
{
	GameData *gamedata = &game->gamedata;
	Tokenizer tokenizer = {0};
	String8 gamedataStr = {0};
	Token token = {0};
	int success = 0;
	MonsterType *monsterType = NULL;
	ItemType *itemType = NULL;

	if (readWholeFile("gamedata", &gamedataStr))
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
		printf("    vit: %d\n", monsterType->vit);
		monsterType = monsterType->next;
	}

	while (itemType != NULL)
	{
		printf("ITEM\n");
		printf(
			"    name: %.*s\n",
			itemType->name.len,
			itemType->name.buf);
		printf("    doesRegen: %s\n", itemType->doesRegen ? "true" : "false");
		printf("    doesPoison: %s\n", itemType->doesPoison ? "true" : "false");
		printf(
			"    hp: %dd%d+%d\n",
			itemType->hp.amount,
			itemType->hp.sides,
			itemType->hp.add);
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
	int i = 0, j = 0;

	for (i = 0; i < (int)game->enemiesLen; i++)
	{
		while (game->enemies[i].hp <= 0 && ((int)game->enemiesLen) > i)
		{
			for (j = i; j < ((int)game->enemiesLen) - 1; j++)
			{
				game->enemies[j] = game->enemies[j + 1];
			}
			game->enemiesLen -= 1;
		}
	}
}

int getMonsterItemCount(Monster *monster)
{
	int count = 0;
	Item *item = monster->inventory;
	while (item != NULL) {
		item = item->next;
		count++;
	}
	return count;
}

Item *getMonsterItem(Monster *monster, int index)
{
	int count = 0;
	Item *item = monster->inventory;
	while (count != index) {
		item = item->next;
		count++;
	}
	return item;
}

void itemAffectMonster(Item *item, Monster *monster)
{
	ItemType *itemType = item->type;
	monster->hp += rollDice(itemType->hp);
	if (itemType->doesRegen)
	{
		monster->regenTimer += 2 + rand() % 5;
	}
	if (itemType->doesPoison)
	{
		monster->poisonTimer += 2 + rand() % 5;
	}
}

void gameBattle(Game *game)
{
	int option = 0;
	int damage = 0;
	bool getAttacked = true;
	bool fled = false;
	uint monsterIndex = 0;
	int hpdiff = 0;

	Monster *enemy = NULL;
	MonsterType *monsterType = NULL;

	uint itemIndex = 0;
	Item *item = NULL;

	printf("O que deseja fazer?\n");
	printf("1. Atacar monstro\n");
	printf("2. Usar item\n");
	printf("3. Jogar item\n");
	printf("4. Fugir\n");
	printf("5. Ver monstros\n");
	option = getInputNumber(1, 5);
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
			damage = monsterType->str * 2 + rand() % 6;

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
			printf("usar qual item?\n");
			item = game->player.inventory;
			itemIndex = 0;

			smallDelay();
			while (item != NULL)
			{
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
			smallDelay();
			option = getInputNumber(1, getMonsterItemCount(&game->player)) - 1;
			itemAffectMonster(getMonsterItem(&game->player, option), &game->player);
			removeItemFromInventoryIndex(&game->player, option);
		}
		break;
		case 3:
		{
			printf("usar qual item?\n");
			item = game->player.inventory;
			itemIndex = 0;

			smallDelay();
			while (item != NULL)
			{
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
			smallDelay();
			option = getInputNumber(1, getMonsterItemCount(&game->player)) - 1;
			printf("em qual monstro?\n");

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

			itemAffectMonster(
				getMonsterItem(&game->player, option),
				&game->enemies[getInputNumber(1, game->enemiesLen) - 1]
			);
			removeItemFromInventoryIndex(&game->player, option);
		}
		break;
		case 4:
		{
			if (rand() % 10 < 8)
			{
				printf("voce conseguiu fugir!\n");
				fled = true;
				smallDelay();
			} else {
				printf("voce nao conseguiu fugir!\n");
				smallDelay();
			}
		}
		break;
		case 5:
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
	if (!fled && getAttacked)
	{
		smallDelay();

		/* loop for poison and regen */
		for (monsterIndex = 0; monsterIndex < game->enemiesLen; monsterIndex++)
		{
			enemy = &game->enemies[monsterIndex];
			if (enemy->poisonTimer > 0)
			{
				enemy->poisonTimer -= 1;

				printf("'%.*s' esta envenenado!\n", enemy->type->name.len, enemy->type->name.buf);
				smallDelay();

				hpdiff = 1 + rand() % 3;
				printf("'%.*s' perde %d hp!\n", enemy->type->name.len, enemy->type->name.buf, hpdiff);
				enemy->hp -= hpdiff;
			}
			if (enemy->regenTimer > 0)
			{
				enemy->regenTimer -= 1;

				printf("'%.*s' esta regenerando hp!\n", enemy->type->name.len, enemy->type->name.buf);
				smallDelay();

				hpdiff = 1 + rand() % 3;
				printf("'%.*s' ganha %d hp!\n", enemy->type->name.len, enemy->type->name.buf, hpdiff);
				enemy->hp += hpdiff;
			}
		}

		removeDeadEnemies(game);

		for (monsterIndex = 0; monsterIndex < game->enemiesLen; monsterIndex++)
		{
			enemy = &game->enemies[monsterIndex];
			if (enemy->hp <= 0)
			{
				continue;
			}

			monsterType = enemy->type;
			damage = MIN(1, monsterType->str / 2 + rand() % 3);
			printf("o monstro '%.*s' ataca!\n", monsterType->name.len, monsterType->name.buf);
			smallDelay();
			printf("o heroi perde %d de hp!\n\n", damage);
			game->player.hp -= damage;
			smallDelay();
		}
	}
	if (fled)
	{
		game->enemiesLen = 0;
	}
	else
	{
		removeDeadEnemies(game);
	}
}

int itemTypesLen(Game *game)
{
	int count = 0;
	ItemType *itemType = game->gamedata.itemTypesHead;
	while (itemType != NULL)
	{
		itemType = itemType->next;
		count++;
	}
	return count;
}


ItemType *getItemType(Game *game, int index)
{
	int count = 0;
	ItemType *itemType = game->gamedata.itemTypesHead;
	while (itemType != NULL)
	{
		if (count == index) {
			return itemType;
		}
		count++;
		itemType = itemType->next;
	}
	assert(0);
}

void findItem(Game *game)
{
	int index = rand() % itemTypesLen(game);
	ItemType *it = getItemType(game, index);

	printf("voce achou um item! '%.*s'!\n", it->name.len, it->name.buf);
	addItemTypeToInventory(&game->player, it);
}

void writeToHighscore(Game *game)
{
	FILE *fp = fopen("highscore", "a");
	if (fp == NULL)
	{
		printf("nao foi possivel escrever ao highscore\n");
	}
	fprintf(fp, "%d\t\t%.*s\n", game->score, game->playername.len, game->playername.buf);
	fclose(fp);
	printf("escrito no arquivo highscore!\n");
}

void removeItems(Game *game)
{
	Item *item = game->player.inventory;
	Item *prev = NULL;
	while (item != NULL)
	{
		if (prev != NULL)
		{
			free(prev);
		}
		prev = item;
		item = item->next;
	}
	if (item != NULL)
	{
		free(item);
	}
}

int gameUpdate(Game *game)
{
	uint i = 0;
	int option = 0;
	int mCount = monsterTypeCount(&game->gamedata);

	if (game->enemiesLen == 0) {
		findItem(game);
		game->score += 1;

		printf("novos inimigos aparecem!\n");
		game->enemiesLen = rand() % 6;

		for (i = 0; i < game->enemiesLen; i++)
		{
			game->enemies[i].type = getMonsterType(&game->gamedata, rand() % mCount);
			game->enemies[i].hp = game->enemies[i].type->vit;
		}
	}

	if (game->player.hp < game->player.type->vit)
	{
		game->player.hp = MIN(game->player.hp + 10, game->player.type->vit);
	}
	else
	{
		game->player.hp -= 1;
	}

	if (game->player.hp <= 0)
	{
		removeItems(game);
		printf("voce morreu!\n");
		return 1;
	}

	gameBattle(game);

	printf("voce deseja sair do jogo?\n1. continuar jogando\n2. sair\n");
	option = getInputNumber(1, 2);
	if (option == 2) {
		return 1;
	}

	return 0;
}

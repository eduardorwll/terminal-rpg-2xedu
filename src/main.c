#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/types.h"
#include "../include/string8.h"
#include "../include/arena.h"
#include "../include/util.h"
#include "../include/game.h"
#include "../include/rng.h"

int main()
{
	int i = 0;

	Game game = {0};
	game.arena.capacity = 1024 * 16; /* 16kb should be enough for everyone ;) */
	game.arena.memory = malloc(game.arena.capacity);
	parseGameData(&game);
	printGameData(&game.gamedata);

	initRNG();
	initMainMenu();

	game.player.type = findMonsterType(&game.gamedata, S8("humano"));
	game.enemiesLen = 3;
	game.enemies[0].type = findMonsterType(&game.gamedata, S8("humano"));
	game.enemies[0].hp = 20;
	game.enemies[1].type = findMonsterType(&game.gamedata, S8("orc"));
	game.enemies[1].hp = 20;
	game.enemies[2].type = findMonsterType(&game.gamedata, S8("elfo"));
	game.enemies[2].hp = 20;

	while (true)
	{
		gameUpdate(&game);
	}
}

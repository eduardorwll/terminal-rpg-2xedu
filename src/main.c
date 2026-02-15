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

	Game game = {0};
	game.arena.capacity = 1024 * 16; /* 16kb should be enough for everyone ;) */
	game.arena.memory = malloc(game.arena.capacity);
	parseGameData(&game);
	printGameData(&game.gamedata);

	initRNG();

	initMainMenu();
}

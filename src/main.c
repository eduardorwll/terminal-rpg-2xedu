#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "string8.h"
#include "arena.h"
#include "util.h"
#include "game.h"

int main()
{
	Game game = {0};
	game.arena.capacity = 1024 * 16; /* 16kb should be enough for everyone ;) */
	game.arena.memory = malloc(game.arena.capacity);
	parseGameData(&game);
	printGameData(&game.gamedata);

    return 0;
}

#include "rng.h"

#include <time.h>
#include <stdlib.h>

void initRNG()
{
    srand(time(NULL));
}

int genRandomInt(int max)
{
    if (max <= 0)
    {
        return 0;
    };

    return (rand() % max) + 1;
}

int rollDice(Dice dice)
{
    int result = 0;
    int c = 0;

    for (c = 0; c < dice.amount; c++)
    {
        result += genRandomInt(dice.sides);
    }

    return result + dice.add;
}
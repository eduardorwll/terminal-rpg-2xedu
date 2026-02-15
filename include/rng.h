#if !defined(RNG_H)
#define RNG_H

void initRNG(void);

int genRandomInt(int max);

struct Dice
{
    int sides;
    int amount;
    int add;
};

typedef struct Dice Dice;

int rollDice(Dice dice);

#endif /* !defined(RNG_H) */
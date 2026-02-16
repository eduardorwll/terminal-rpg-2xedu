#include <stdio.h>
#include <string.h>

#include "../include/menu.h"

#include "../include/game.h"
#include "../include/string8.h"

void printMenu(String8Array *options, char *menuTitle)
{
    int i = 1;

    printf("\n=========================\n\n");

    puts(menuTitle);

    for (i = 1; i <= options->count; i++)
    {
        printf("\n%d. %.*s\n", i, options->strings[i - 1].len, options->strings[i - 1].buf);
    }

    printf("\n=========================\n\n");

    printf("Escolha: ");
}

void initMainMenu(Game *game)
{
    char *menuTitle = "Menu principal";
    int arrLen = 1;
    char *strOptions[] = {"Jogar"};
    int choice = 0;

    String8Array options = S8Arr(strOptions, arrLen);

    printMenu(&options, menuTitle);

    for (;;)
    {
        scanf("%d", &choice);
        if (choice == 1)
        {
            createHero(game);
            break;
        }
        else
        {
            printf("\nResposta invalida. Responda novamente:\n");
            printMenu(&options, menuTitle);
        }
    }
}

void createHero(Game *game)
{
    char *menuTitle = "Crie seu herói";
    int arrLen = 3;
    char *strOptions[] = {"Nome", "Tipo", "Iniciar campanha"};
    int choice = 0;

    String8Array options = S8Arr(strOptions, arrLen);

    printMenu(&options, menuTitle);

    game->playername.buf = NULL;
    game->player.type = NULL;

    for (;;)
    {
        scanf("%d", &choice);

        if (choice == 1)
        {
            chooseHeroName(game);
            printMenu(&options, menuTitle);
        }
        else if (choice == 2)
        {
            chooseHeroType(game);
            printMenu(&options, menuTitle);
        }
        else if (choice == 3 && game->player.type != NULL && game->playername.buf != NULL)
        {

            break;
        }
        else
        {
            printf("\nResposta invalida. Responda novamente:\n");
            printMenu(&options, menuTitle);
        }
    }
}

void chooseHeroName(Game *game)
{
    char *menuTitle = "De um nome ao seu herói";
    static char playername[50] = {0};

    printf("\n=========================\n\n");

    puts(menuTitle);

    printf("\n=========================\n\n");

    printf("Escolha: ");

    scanf("%s", playername);

    game->playername = S8(playername);

    printf("%s", game->playername.buf);
}

void chooseHeroType(Game *game)
{
    char *menuTitle = "Escolha o tipo do seu herói";
    int choice = 0;
    int arrLen = 8;
    char *strOptions[] = {"Humano", "Elfo", "Duende", "Orc", "Esqueleto", "Slime", "Ciclope", "Minotauro"};

    String8Array options = S8Arr(strOptions, arrLen);

    printMenu(&options, menuTitle);

    for (;;)
    {
        scanf("%d", &choice);
        if (choice <= arrLen && choice > 0)
        {
            switch (choice)
            {
            case 1:
                game->player.type = findMonsterType(&game->gamedata, S8("humano"));
                break;
            case 2:
                game->player.type = findMonsterType(&game->gamedata, S8("elfo"));
                break;
            case 3:
                game->player.type = findMonsterType(&game->gamedata, S8("duende"));
                break;
            case 4:
                game->player.type = findMonsterType(&game->gamedata, S8("orc"));
                break;
            case 5:
                game->player.type = findMonsterType(&game->gamedata, S8("esqueleto"));
                break;
            case 6:
                game->player.type = findMonsterType(&game->gamedata, S8("slime"));
                break;
            case 7:
                game->player.type = findMonsterType(&game->gamedata, S8("ciclope"));
                break;
            case 8:
                game->player.type = findMonsterType(&game->gamedata, S8("minotauro"));
                break;
            }
            break;
        }
        else
        {
            printf("\nResposta invalida. Responda novamente:\n");
            printMenu(&options, menuTitle);
        }
    }
}
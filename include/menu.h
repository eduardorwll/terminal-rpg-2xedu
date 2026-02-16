#if !defined(MENU_H)
#define MENU_H

#include "string8.h"
#include "game.h"

void initMainMenu(Game *game);
void printMenu(String8Array *options, char *menuTitle);
void createHero(Game *game);
void chooseHeroName(Game *game);
void chooseHeroType(Game *game);

#endif /* !defined(MENU_H) */

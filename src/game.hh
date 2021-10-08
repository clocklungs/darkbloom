#ifndef GAME_HH
#define GAME_HH

#include <SDL/SDL.h>
#include "font.hh"

#define GAMENAME "d4|2kbl00m"
#define GAMEVERSION "alpha 0.06"

extern SDL_Surface *screen, *menuBackground, *sideBar;
extern Font font, menuFont, menuHighlightFont, popupFont, popupsmallFont;
extern Font dboxFont, sideBarFont, sideBarStatusFont;
extern bool game_started;

void initGraphics();
void startGame();
void continueGame();
void gameLoop();
void gameOver();
void worldLoop();

#endif

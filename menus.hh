#ifndef MENUS_HH
#define MENUS_HH

#include "font.hh"

extern SDL_Surface *screen, *menuBackground;
extern Font font, menuFont, menuHighlightFont, popupFont, popupsmallFont;

int mainMenu();
int optionMenu(int curr);

#endif

#ifndef DBOX_HH
#define DBOX_HH

#include "dialog.hh"
#include "font.hh"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>

#define DBOX_MAXLINES 5
#define DBOX_CHOICES DBOX_MAXLINES - 1

#define DBOX_TOP 10
#define DBOX_LEFT 10

#define DBOX_TEXTMAX 26
#define DBOX_TEXTLEFT 110
#define DBOX_TEXTTOP 50

#define DBOX_TITLEMAX 17
#define DBOX_TITLELEFT 13
#define DBOX_TITLETOP 5
#define DBOX_TITLEDEFAULT ""

#define DBOX_IMGLEFT 8
#define DBOX_IMGTOP 39

extern SDL_Surface *dbox, *dbox_back;
extern Font dboxFont, dboxHighlightFont;
extern char dbox_lines[DBOX_MAXLINES][DBOX_TEXTMAX + 1];
extern int dbox_line;
extern Mix_Chunk *dbox_snd;

int dboxChoice(const char *title, Choice choices[DBOX_CHOICES]);
void dboxText();
void dboxDraw();
void dboxErase();

#endif

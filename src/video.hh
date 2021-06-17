#ifndef VIDEO_HH
#define VIDEO_HH

#include <stdlib.h>
#include "SDL.h"
#include "config.hh"

#define MSECS_PER_FRAME 1000/20

/*global variables*/
extern SDL_Surface *screen; /*display surface*/
extern unsigned int msecs_per_frame;

void initSDL();
void initVideo(const char *title);
void changeModeSDL();
void toggleFullscreen();
void flushEvents();
bool flushJunkEvents(SDL_Event *event);
void delay(unsigned int ms, bool escapeable=true);
void updateDisplay();
int checkToggles(SDL_keysym key);

#endif

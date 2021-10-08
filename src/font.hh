#ifndef FONT_HH
#define FONT_HH


#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


typedef struct {
  SDL_Surface *font;
  int w;
  int h;
} Font;


void initFont(Font *font, const char *filename);
void freeFont(Font *font);
int writeFont(Font *font, SDL_Surface *target, int x, int y, const char *text);
int centerFont(Font *font, SDL_Surface *target, int x, int y, const char *text);
int rightFont(Font *font, SDL_Surface *target, int x, int y, const char *text);
int popUpFont(Font *font, SDL_Surface *target, const char *text);
int popXYFont(Font *font, SDL_Surface *target, int x, int y, const char *text);

#endif

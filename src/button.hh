#ifndef BUTTON_HH
#define BUTTON_HH

#include <SDL2/SDL.h>

typedef struct {
  int top,
      left,
      bottom,
      right;
  int active;
} Button;

int clicked(SDL_MouseButtonEvent *mouse, Button button);
int mouseOver(SDL_MouseButtonEvent *mouse, Button button);

#endif

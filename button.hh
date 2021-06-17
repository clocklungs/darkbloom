#ifndef BUTTON_HH
#define BUTTON_HH

#include "SDL.h"

typedef struct Button {
  int top,
      left,
      bottom,
      right;
  int active;
  };

int clicked(SDL_MouseButtonEvent *mouse, Button button);
int mouseOver(SDL_MouseButtonEvent *mouse, Button button);

#endif

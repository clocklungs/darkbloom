#include "button.hh"

int clicked(SDL_MouseButtonEvent *mouse, Button button) {
    if (mouse->x >= button.left && mouse->x <= button.right && mouse->y >= button.top && mouse->y <= button.bottom) {
        return 1;
    }
    return 0;
}

int mouseOver(SDL_MouseButtonEvent *mouse, Button button) { return clicked(mouse, button); }

/*EOF*/

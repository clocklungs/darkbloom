#include "font.hh"

/*load font from file and initialize font metrics*/
void initFont(Font *font, const char *filename) {
    font->font = IMG_Load(filename);
    if (font->font == NULL) {
        fprintf(stderr, "error loading font: %s\n", SDL_GetError());
        exit(1);
    }
    font->h = font->font->h;
    font->w = font->font->w / 94;
}

/*free surface memory used by font*/
void freeFont(Font *font) {
    SDL_FreeSurface(font->font);
    font->w = 0;
    font->h = 0;
}

/*writeFont:
takes a font and string 'text' and blits it to the 'target' surface
at the specified x, y coordinates
returns: -1 on error, or the width of the written text in pixels*/
int writeFont(Font *font, SDL_Surface *target, int x, int y, const char *text) {
    /*local variable declarations*/
    SDL_Rect src, dst;
    int i;

    /*check for existence of font*/
    if (font == NULL) {
        fprintf(stderr, "error writing text: font does not exist\n");
        return -1;
    }

    /*check for existence of surfaces*/
    if (font->font == NULL) {
        fprintf(stderr, "error writing text: font not initialized\n");
        return -1;
    }

    if (target == NULL) {
        fprintf(stderr, "error writing text: target surface not exist\n");
        return -1;
    }

    src.y = 0;
    src.w = font->w;
    src.h = font->h;
    dst.y = y;

    for (i = 0; text[i] != '\0'; ++i) {
        /*don't bother blitting spaces*/
        if (text[i] == ' ')
            continue;
        /*set destination x*/
        dst.x = x + i * font->w;
        /*print character if it is a printable character*/
        if (text[i] >= '!' && text[i] <= '~') {
            src.x = (text[i] - '!') * font->w;
            if (SDL_BlitSurface(font->font, &src, target, &dst) == -1)
                return -1;
        }
    }

    /*return width of written text*/
    return i * font->w;
}

/*draws text centered at x, y position, returns -1 on error*/
int centerFont(Font *font, SDL_Surface *target, int x, int y, const char *text) {
    int i;

    if (font == NULL) {
        fprintf(stderr, "error writing text: font does not exist\n");
        return -1;
    }

    i = strlen(text);
    i *= font->w;
    return writeFont(font, target, x - (i / 2), y, text);
}

/*draws text right justified at x, y position, returns -1 on error*/
int rightFont(Font *font, SDL_Surface *target, int x, int y, const char *text) {
    int i;

    if (font == NULL) {
        fprintf(stderr, "error writing text: font does not exist\n");
        return -1;
    }

    i = strlen(text);
    i *= font->w;
    return writeFont(font, target, x - i, y, text);
}

/*pops up a 'box' w/ the text in the
center of the target. returns less than zero on error*/
int popUpFont(Font *font, SDL_Surface *target, const char *text) {
    int i;
    SDL_Rect box;

    if (font == NULL) {
        fprintf(stderr, "error writing text: font does not exist\n");
        return -1;
    }

    i = strlen(text);
    i *= font->w;

    /*
    SDL_FillRect(target, NULL, SDL_MapRGB(target->format, 0x00, 0x00, 0x00));
    */

    box.w = i + 20;
    box.h = font->h + 20;
    box.x = target->w / 2 - box.w / 2;
    box.y = target->h / 2 - box.h / 2;

    /*box border color*/
    SDL_FillRect(target, &box, SDL_MapRGB(target->format, 0x66, 0x66, 0x66));

    box.w -= 4;
    box.h -= 4;
    box.x += 2;
    box.y += 2;

    /*inner box color*/
    SDL_FillRect(target, &box, SDL_MapRGB(target->format, 0x44, 0x44, 0x44));

    return centerFont(font, target, target->w / 2, target->h / 2 - font->h / 2, text);
}

/*pops up a 'box' w/ the text at the x, y coords in target.
returns less than zero on error*/
int popXYFont(Font *font, SDL_Surface *target, int x, int y, const char *text) {
    int i;
    SDL_Rect box;

    if (font == NULL) {
        fprintf(stderr, "error writing text: font does not exist\n");
        return -1;
    }

    i = strlen(text);
    i *= font->w;

    /*
    SDL_FillRect(target, NULL, SDL_MapRGB(target->format, 0x00, 0x00, 0x00));
    */

    box.w = i + 20;
    box.h = font->h + 20;
    box.x = x - box.w / 2;
    box.y = y - box.h / 2;

    SDL_FillRect(target, &box, SDL_MapRGB(target->format, 0x66, 0x55, 0x22));

    box.w -= 4;
    box.h -= 4;
    box.x += 2;
    box.y += 2;

    SDL_FillRect(target, &box, SDL_MapRGB(target->format, 0xFF, 0xEE, 0xBB));

    return centerFont(font, target, x, y - font->h / 2, text);
}

/*EOF*/

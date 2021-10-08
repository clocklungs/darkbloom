/*********************************************************************
p.carley (pcarley@lssu.edu)

modified:
  Fri Jul 22 22:46:33 EDT 2005

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "button.hh"
#include "config.hh"
#include "dbox.hh"
#include "dialog.hh"
#include "font.hh"
#include "game.hh"
#include "intro.hh"
#include "menus.hh"
#include "object.hh"
#include "plyr.hh"
#include "sound.hh"
#include "video.hh"
#include "world.hh"

/*global variables*/
SDL_Surface *screen, *menuBackground, *world_image, *sideBar;
SDL_Surface *dbox, *dbox_back;
SDL_Rect screen_display, world_size;
Mix_Music *music;
Mix_Chunk *samples[SAMPLES];
Mix_Chunk *dbox_snd;
Font font, menuFont, menuHighlightFont, popupFont, popupsmallFont;
Font dboxFont, dboxHighlightFont, sideBarFont, sideBarStatusFont;
Config config;
ObjectList objects;
World world;
AnimatedObject player;

std::string areaname;
int gold;
int dbox_line;
unsigned int msecs_per_frame = MSECS_PER_FRAME;
char dbox_lines[DBOX_MAXLINES][DBOX_TEXTMAX + 1];
bool game_started = false;

void showVersionInfo();

int main(int argc, char **argv) {

    /*just for giggles*/
    if (argc > 1 && !strcmp("--version", argv[1])) {
        showVersionInfo();
        return 0;
    }

    loadConfig();

    initSDL();

    initGraphics();

    initVideo(GAMENAME);

    initSound();

    showIntro();

    /*respawn main menu until exit*/
    while (mainMenu())
        ;

    saveConfig();

    showOutro();

    /*shut down sdl*/
    SDL_Quit();

    return 8008135;
}

void showVersionInfo() {
    SDL_version compile;
    SDL_version linked;

    /*display version info*/
    printf("%s - %s\n", GAMENAME, GAMEVERSION);

    /*display SDL version info*/
    SDL_VERSION(&compile);
    SDL_GetVersion(&linked);
    printf("SDL %d.%d.%d(compiled)/%d.%d.%d (linked)\n", compile.major, compile.minor, compile.patch, linked.major, linked.minor, linked.patch);

    /*display SDL_mixer version info*/
    MIX_VERSION(&compile);
    const SDL_version *link = Mix_Linked_Version();
    printf("SDL_mixer %d.%d.%d(compiled)/%d.%d.%d (linked)\n", compile.major, compile.minor, compile.patch, link->major, link->minor, link->patch);
}

/*EOF*/

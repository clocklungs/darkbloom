#include <stdlib.h>
#include <stdio.h>

#include "SDL_image.h"

#include "menus.hh"
#include "config.hh"
#include "video.hh"
#include "game.hh"
#include "sound.hh"
#include "button.hh"
#include "dialog.hh"


int mainMenu()
{
  SDL_Event event;
  Font *font, *hlfont;
  int i, top, left, items, width, prev, selected;
  static int curr=0;
  char *text[] = {
                "New game",
                "Continue",
                "Options",
                "Quit",
                "END"};
  Button menuItem[4];

  for(i=0; strcmp(text[i], "END"); ++i);

  prev     = curr;
  selected = -1;
  items    = i;
  font     = &menuFont;
  hlfont   = &menuHighlightFont;

  top    = screen->h/2 - (font->h * items)/2;
  left   = screen->w/2;

  /*draw menu background*/
  SDL_BlitSurface(menuBackground, NULL, screen, NULL);

  /*draw menu*/
  for(i=0; i<items; i++)
  {
    /*draw menu item text*/
    width = centerFont(font, screen, left, top+(i*font->h), text[i]);
    /*register menu item as button*/
    menuItem[i].top    = top + (i * font->h);
    menuItem[i].left   = left - width/2;
    menuItem[i].bottom = menuItem[i].top + font->h;
    menuItem[i].right  = left + width/2;
  }

  /*highlight current menu item*/
  centerFont(hlfont, screen, left, top+(curr*font->h), text[curr]);
  updateDisplay();

  /*clear event queue before we start selecting things*/
  flushEvents();

  while(SDL_WaitEvent(&event))
  {
    /*generic quit events*/
    if(event.type == SDL_QUIT) exit(1);

    /*mouse click events*/
    if(event.type == SDL_MOUSEBUTTONUP)
    {
      /*check if they clicked an actual menu item*/
      for(i=0; i<items; ++i)
      {
        if(clicked(&event.button, menuItem[i]))
        {
          selected = i;
          break;
        }
      }
      if(selected == -1 && event.button.button == SDL_BUTTON_RIGHT)
      {
        return 0;
      }
    }

    /*mouse move events*/
    if(event.type == SDL_MOUSEMOTION)
    {
      for(i=0; i<items; ++i)
      {
        if(mouseOver(&event.button, menuItem[i]))
        {
          /*don't update the menu if it doesn't need it*/
          if(i != prev) curr = i;
          break;
        }
      }
    }

    /*keypress events*/
    if(event.type == SDL_KEYUP)
    {
      switch(event.key.keysym.sym)
      {
        case SDLK_ESCAPE:
          return 0;
        case SDLK_UP:
          if(curr <= 0) curr = items-1;
          else curr--;
          break;
        case SDLK_DOWN:
          if(curr >= items-1) curr = 0;
          else curr++;
          break;
        case SDLK_RETURN:
        case SDLK_SPACE:
        case SDLK_KP_ENTER:
          selected = curr;
          break;
        default:
          /*toggle music/sound/fullscreen*/
          if(checkToggles(event.key.keysym) == 3) return 1;
          break;
      } /*endof switch(event.key.keysym.sym)*/
    }

    /*check for menu change*/
    if(curr != prev)
    {
      /*play menu change sound*/
      playSample(MENUBLIP);
      /*de-highlight old menu item*/
      centerFont(font, screen, left, top+(prev*font->h), text[prev]);
      prev = curr;
      /*highlight new menu item*/
      centerFont(hlfont, screen, left, top+(curr*font->h), text[curr]);
      /*update display*/
      updateDisplay();
    }

    /*check for menu selection*/
    if(selected > -1)
    {
      /*they selected a menu item*/
      playSample(MENUBLIP);
      switch(selected)
      {
        /*selected start*/
        case 0:
          startGame();
          return 1;

        /*selected continue*/
        case 1:
          continueGame();
          return 1;

        /*selected options*/
        case 2:
          i = 0;
          while(i != -1) i = optionMenu(i);
          return 1;

        /*selected quit*/
        case 3:
          return 0;

        default:
          break;
      }
    } /*endof if(selected > -1)*/

  }
  /*we only get here on a wait event error, maybe we should
  quit instead of reloading the menu*/
  return 1;
}


int optionMenu(int curr) {
  SDL_Event event;
  Font *font, *hlfont;
  int i, top, left, items, width, prev, selected;
  SDLKey key;
  char *text[] = {
                "Fullscreen: ",
                "Sounds: ",
                "Music: ",
                "END"};
  Button menuItem[3];
  int *values[3];

  values[0] = &config.fullscreen;
  values[1] = &config.sounds;
  values[2] = &config.music;

  for(i=0; strcmp(text[i], "END"); ++i);

  prev     = curr;
  selected = -1;
  key      = SDLK_RETURN;
  items    = i;
  font     = &menuFont;
  hlfont   = &menuHighlightFont;

  top    = screen->h /2 - (font->h * items)/2;
  left   = screen->w/2 + 3*font->w;

  /*draw menu background*/
  SDL_BlitSurface(menuBackground, NULL, screen, NULL);

  /*draw menu*/
  for(i=0; i<items; i++) {
    width = rightFont(font, screen, left, top+(i*font->h), text[i]);
    /*register menu item as button*/
    menuItem[i].top    = top + (i * font->h);
    menuItem[i].left   = left - width;
    menuItem[i].bottom = menuItem[i].top + font->h;
    if(*(values[i])) {
      width = writeFont(font, screen, left, top+(i*font->h), "yes");
      } else {
      width = writeFont(font, screen, left, top+(i*font->h), "no");
      }
    menuItem[i].right  = left + width;
    }

  /*highlight current menu item*/
  rightFont(hlfont, screen, left, top+(curr*font->h), text[curr]);
    if(*(values[curr])) {
      writeFont(hlfont, screen, left, top+(curr*font->h), "yes");
       } else {
      writeFont(hlfont, screen, left, top+(curr*font->h), "no");
      }

  updateDisplay();

  while(SDL_WaitEvent(&event)) {
    /*generic quit events*/
    if(event.type == SDL_QUIT) exit(1);

    /*mouse click events*/
    if(event.type == SDL_MOUSEBUTTONUP) {
      /*check if they clicked an actual menu item*/
      for(i=0; i<items; ++i) {
        if(clicked(&event.button, menuItem[i])) {
          selected = i;
          if(event.button.button == SDL_BUTTON_RIGHT) {
            key = SDLK_LEFT;
            } else {
            key = SDLK_RETURN;
            }
          break;
          }
        }
      if(selected == -1 && event.button.button == SDL_BUTTON_RIGHT) {
        return -1;
        }
      }

    /*mouse move events*/
    if(event.type == SDL_MOUSEMOTION) {
      for(i=0; i<items; ++i) {
        if(mouseOver(&event.button, menuItem[i])) {
          /*don't update the menu if it doesn't need it*/
          if(i != prev) curr = i;
          break;
          }
        }
      }

    /*keypress events*/
    if(event.type == SDL_KEYUP) {
      switch(event.key.keysym.sym) {
        case SDLK_ESCAPE:
          return -1;
        case SDLK_UP:
          if(curr <= 0) curr = items-1;
          else curr--;
          break;
        case SDLK_DOWN:
          if(curr >= items-1) curr = 0;
          else curr++;
          break;
        case SDLK_RETURN:
        case SDLK_SPACE:
        case SDLK_KP_ENTER:
        case SDLK_LEFT:
        case SDLK_RIGHT:
          selected = curr;
          key      = event.key.keysym.sym;
          break;
        default:
          /*check music/sound/fullscreen toggles*/
          if(checkToggles(event.key.keysym)) return curr;
          break;
        } /*endof switch(event.key.keysym.sym)*/
      }

    /*check for menu change*/
    if(curr != prev) {
      /*play menu change sound*/
      playSample(MENUBLIP);
      /*de-highlight old menu item*/
      rightFont(font, screen, left, top+(prev*font->h), text[prev]);
      if(*(values[prev])) {
        writeFont(font, screen, left, top+(prev*font->h), "yes");
        } else {
        writeFont(font, screen, left, top+(prev*font->h), "no");
        }
      prev = curr;
      /*highlight new menu item*/
      rightFont(hlfont, screen, left, top+(curr*font->h), text[curr]);
      if(*(values[curr])) {
        writeFont(hlfont, screen, left, top+(curr*font->h), "yes");
        } else {
        writeFont(hlfont, screen, left, top+(curr*font->h), "no");
        }
      /*update display*/
      updateDisplay();
      }

    /*check for menu selection*/
    if(selected > -1) {
      /*they selected a menu item*/
      playSample(MENUBLIP);
      switch(selected) {
        case 0: /*fullscreen*/
          toggleFullscreen();
          break;
        case 1: /*sounds*/
          toggleSounds();
          break;
        case 2: /*music*/
          toggleMusic();
          break;
        default:
          break;
        }
      return selected;
      }
    }

  /*wait event error*/
  return -1;
  }

/*EOF*/

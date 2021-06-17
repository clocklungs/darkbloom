#include <stdio.h>
#include "intro.hh"
#include "font.hh"
#include "video.hh"
#include "game.hh"


void logos();
void intro();
void outro();
void spglogo();
void edlogo();


void showIntro()
{
  logos();
  intro();
}


void showOutro()
{
  outro();
}


void spglogo()
{
  SDL_Surface *spg_logo;
  SDL_Rect dest;

  /*load sweater puppy games logo*/
  spg_logo = IMG_Load("images/spg_logo.png");
  if(spg_logo == NULL)
  {
    fprintf(stderr, "unable to load image: %s\n", IMG_GetError());
    exit(1);
  }

  /*blank screen*/
  SDL_FillRect(screen, NULL, 0);

  dest.x = screen->w/2 - spg_logo->w/2;
  dest.y = screen->h/2 - spg_logo->h/2;
  SDL_BlitSurface(spg_logo, NULL, screen, &dest);
  updateDisplay();
  delay(3750);

  /*blank screen*/
  SDL_FillRect(screen, NULL, 0);

  SDL_FreeSurface(spg_logo);
}


void edlogo()
{
  SDL_Surface *ed_logo;
  SDL_Rect dest;

  /*load sweater puppy games logo*/
  ed_logo = IMG_Load("images/ed_logo.png");
  if(ed_logo == NULL)
  {
    fprintf(stderr, "unable to load image: %s\n", IMG_GetError());
    exit(1);
  }

  /*blank screen*/
  SDL_FillRect(screen, NULL, 0);

  dest.x = screen->w/2 - ed_logo->w/2;
  dest.y = screen->h/2 - ed_logo->h/2;
  SDL_BlitSurface(ed_logo, NULL, screen, &dest);
  updateDisplay();
  delay(2000);

  /*blank screen*/
  SDL_FillRect(screen, NULL, 0);

  SDL_FreeSurface(ed_logo);
}


void logos() {
  Font font;

  initFont(&font, "images/logofont.png");

  spglogo();

  centerFont(&font, screen, screen->w/2, screen->h/2-font.h/2,
             "in association with");
  updateDisplay();
  delay(2000);

  edlogo();

  centerFont(&font, screen, screen->w/2, screen->h/2-font.h/2,
             "present");
  updateDisplay();
  delay(1250);

  freeFont(&font);
  }


void intro()
{
  Font font;

  initFont(&font, "images/logofont.png");

  /*blank screen*/
  SDL_FillRect(screen, NULL, 0);

  centerFont(&font, screen, screen->w/2, screen->h/2-font.h/2,
             GAMENAME);

  updateDisplay();
  delay(5000);

  freeFont(&font);
}


void outro()
{
  Font font;

  initFont(&font, "images/logofont.png");

  /*blank screen*/
  SDL_FillRect(screen, NULL, 0);

  centerFont(&font, screen, screen->w/2, screen->h/2-font.h*2,
             GAMENAME);
  centerFont(&font, screen, screen->w/2, screen->h/2+font.h,
             "(c) 2005, Sweater Puppy Games");
  centerFont(&font, screen, screen->w/2, screen->h/2+font.h*2,
             "Thanks for playing!");


  updateDisplay();
  delay((unsigned int)-1);

  freeFont(&font);
}


/*EOF*/

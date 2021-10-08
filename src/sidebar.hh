#ifndef SIDEBAR_HH
#define SIDEBAR_HH

#include <string>
#include <SDL/SDL.h>
#include "plyr.hh"
#include "var.hh"


typedef enum STATUS {NORMAL, POISONED, CRITICAL, DEAD} STATUS;


typedef struct Status
{
  char name[10];
  char Class[10];
  char weapon[10];
  int level;
  int hp;
  int max_hp;
  int mp;
  int max_mp;
  STATUS status;
  bool active;

  std::string idle_img;
  std::string attack_img;
  std::string magic_img;

  int xp;
  int strength;
  float strength_mod;
  float hp_mod;
  float mp_mod;
} Status;


extern SDL_Surface *sideBar;
extern VAR_paramList sidebar_vars;
extern int gold;
extern Status status[7];

void sidebarUpdate(bool force=false);

#endif

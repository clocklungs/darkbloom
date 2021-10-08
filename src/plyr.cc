#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "plyr.hh"
#include "world.hh"
#include "sidebar.hh"


/*this will not be here for long
just set up some sample stats, so we have something to display*/
void stats_init()
{
  //printf("DEBUG: init stats\n");
  gold = 1134;
  /*set images for player names*/
  sidebar_vars.set("Asterisk", "string", "images/elf_girl_small.png");
  sidebar_vars.set("Yddor", "string", "images/cat_sailor_small.png");
  sidebar_vars.set("Paul", "string", "images/cat_sailor_small.png");
  /*set images for weapon names*/
  sidebar_vars.set("dagger", "string", "images/dagger.png");
  sidebar_vars.set("dagger2", "string", "images/dagger_big.png");
  sidebar_vars.set("sword", "string", "images/dualkatanas.png");
  sidebar_vars.set("wang", "string", "images/wang.png");

  strcpy(status[0].name, "Asterisk");
  strcpy(status[0].Class, "Rogue");
  strcpy(status[0].weapon, "dagger");
  status[0].level = 1;
  status[0].hp = 69;
  status[0].max_hp = 69;
  status[0].mp = 38;
  status[0].max_mp = 38;
  status[0].status = NORMAL;
  status[0].active = true;
  status[0].idle_img="images/battler2.png";
  status[0].attack_img="images/battler2attack.png";
  status[0].magic_img="images/battler2magic.png";
  status[0].strength=15;
  status[0].strength_mod=(int)(status[0].strength+status[0].level*1.2);
  status[0].xp=0;
  status[0].strength_mod=status[0].strength+status[0].level*1.2;
  status[0].strength_mod=(int)(status[0].strength+status[0].level*1.2);
  status[0].hp_mod=(int)(status[0].max_hp+status[0].level*1.5);
  status[0].mp_mod=(int)(status[0].max_mp+status[0].level*1.4);

  strcpy(status[1].name, "Yddor");
  strcpy(status[1].Class, "Fighter");
  strcpy(status[1].weapon, "sword");
  status[1].level = 1;
  status[1].hp = 63;
  status[1].max_hp = 63;
  status[1].mp = 42;
  status[1].max_mp = 42;
  status[1].status = NORMAL;
  status[1].active = true;
  status[1].idle_img="images/battler1.png";
  status[1].attack_img="images/battler1attack.png";
  status[1].magic_img="images/battler1magic.png";
  status[1].strength=14;
  status[1].xp=0;
  status[1].strength_mod=(int)(status[1].strength+status[1].level*1.2);
  status[1].hp_mod=(int)(status[1].max_hp+status[1].level*1.4);
  status[1].mp_mod=(int)(status[1].strength+status[1].level*1.3);

  strcpy(status[2].name, "Paul");
  strcpy(status[2].Class, "id10t");
  strcpy(status[2].weapon, "wang");
  status[2].level = 1;
  status[2].hp = 66;
  status[2].max_hp = 66;
  status[2].mp = 0;
  status[2].max_mp = 50;
  status[2].status = NORMAL;
  status[2].active = true;
  status[2].idle_img="images/cat_sailor_fight3.png";
  status[2].attack_img="images/battler0attack.png";
  status[2].magic_img="images/battler0magic.png";
  status[2].strength=13;
  status[2].xp=0;
  status[2].strength_mod=status[2].strength+status[2].level*1.2;
  status[2].strength_mod=(int)(status[2].strength+status[2].level*1.2);
  status[2].hp_mod=(int)(status[2].max_hp+status[2].level*1.5);
  status[2].mp_mod=(int)(status[2].max_mp+status[2].level*1.4);
}



void player_init()
{
  if(!player.load("objects/yddor.anim"))
  {
    fprintf(stderr, "error loading player\n");
    exit(1);
  }
  player.setSpeed(RUN);
}

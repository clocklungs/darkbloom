/*******************************************
*sweater puppy games w/ ED
*Darkbloom 574|2
*battle.cc
********************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "SDL.h"
#include "SDL_image.h"
#include "battle.hh"
#include "font.hh"
#include "sidebar.hh"
#include "enemy.hh"
#include "video.hh"

/*************************************************************
*This .cc will need to handle
*
*#attacking - be it the player moving over to the enemy or enemy
*over to player and performing there attack.
*
*#set_up_attack - set up what each charecter is doing ie real time
*determine
*
*#calculate hit with appropritate dmg.
*
*#display batle menu for availble charecter.
*       display abilities.
*
*#whos turn nmy/plyr
*
*#set battle screen
*   -battle image.(plyr|nmy)
*   -display battle background
*   
*#victory screen - gold given, did plyr lvl(not sure if applicable)
*                  return screen.
*#death
****************************************************************/


int currplayers, currenemies;
bworld battle;
bcusor cursor, pcursor;
SDL_Rect cell0, cell1, cell2, cell3, bosscell;
bplayers battle_players[MAXPLYRS];
bnmys battle_enemies[MAXNMYS];
Font battlefont;
Font highlight;
Font heal;
char bchoice0[BMENUCHOICES][50];
char bchoice1[MAGICS][50];
spellsani spell[MAGICS];


void battlefield_init(int nmy,int boss)
  {
  int random_enemy;
  char player_battle_image[50];
  int i, usedcell[MAXNMYS];

  srand(time(NULL));/*seed random*/

  strcpy(bchoice0[0],"attack");
  strcpy(bchoice0[1],"magic");
  //strcpy(bchoice0[2],"item");
  strcpy(bchoice1[0],"fire");
  strcpy(bchoice1[1],"ice");
  strcpy(bchoice1[2],"nature");
  initFont(&battlefont,"images/font.png");
  initFont(&highlight,"images/sidebarstatusfont.png");
  initFont(&heal,"images/healbutcheredpaulsfont.png");
  battle.image=IMG_Load("images/battle_field1.png");
  battle.bmenu0=IMG_Load("images/bmenu.png");
  battle.bmenu1=IMG_Load("images/bmenu.png");

  cell0.x=135;
  cell0.y=100;
  cell1.x=25;
  cell1.y=155;
  cell2.x=25;
  cell2.y=310;
  cell3.x=135;
  cell3.y=310;
  
  bosscell.x=75;
  bosscell.y=225;
  
  for(i=0;i<MAXNMYS;i++)
      {
      usedcell[i]=0;
      }
  battle.bmenu_rect.x=BMENU1X;
  battle.bmenu_rect.y=BMENU1Y;
  
  if(battle.image==NULL)
    {
    fprintf(stderr,"error loading image: %s\n",IMG_GetError());
    }
  if(battle.bmenu0==NULL)
    {
    fprintf(stderr,"error loading image: %s\n",IMG_GetError());
    }
  if(battle.bmenu1==NULL)
    {
    fprintf(stderr,"error loading image: %s\n",IMG_GetError());
    }
    
  battle.rect.x=0;
  battle.rect.y=0;

  /*this only needs to be here if we overwrote the sidebar area*/
  //sidebarUpdate(true);
  
  for(i=0;i<MAGICS;i++)
    {/*laods magic*/
    sprintf(player_battle_image,"images/magic%d.png",i);
    spell[i].magic=IMG_Load(player_battle_image);
    if(spell[i].magic==NULL)
      {
      fprintf(stderr,"error loading image: %s\n",IMG_GetError());
      }
    spell[i].spell.x=MAGICX;
    spell[i].spell.y=MAGICY;
    spell[i].spell.h=MAGICH;
    spell[i].spell.w=MAGICW;
    }
  
  currplayers=0;
  for(i=0;i<MAXPLYRS;i++)/*FIX:change to current amount of plyrs*/
    {/*loads current players*/
    if(status[i].active==true)
      {
      currplayers++;
      battle_players[i].image[0]=IMG_Load(status[i].idle_img.c_str());
      battle_players[i].image[1]=IMG_Load(status[i].attack_img.c_str());
      battle_players[i].image[2]=IMG_Load(status[i].magic_img.c_str());

      if(battle_players[i].image[0]==NULL)
        {
        fprintf(stderr,"error loading image: %s\n",IMG_GetError());
        }
      if(battle_players[i].image[1]==NULL)
        {
        fprintf(stderr,"error loading image: %s\n",IMG_GetError());
        }
      if(battle_players[i].image[2]==NULL)
        {
        fprintf(stderr,"error loading image: %s\n",IMG_GetError());
        }
      battle_players[i].plyr_rect.x=0;/*starting x*/
      battle_players[i].plyr_rect.y=0;/*starting y*/
      battle_players[i].plyr_rect.h=PLYRIMGHEIGHT;
      battle_players[i].plyr_rect.w=PLYRIMGWIDTH;
      battle_players[i].attacking=0;
      if(i==0)
        {
        battle_players[i].wrect.x=PLYRX;
        battle_players[i].wrect.y=PLYRY;
        }
      else
        {
        battle_players[i].wrect.x=battle_players[i-1].wrect.x+PLYRXMOVE;
        battle_players[i].wrect.y=battle_players[i-1].wrect.y+
                                  (battle_players[i-1].plyr_rect.h);
        }
      }
    }
   /*HARDCODE*/
   battle_players[0].delay=9;
   battle_players[0].delaymax=10;
   battle_players[1].delay=3;
   battle_players[1].delaymax=11;
   battle_players[2].delay=6;
   battle_players[2].delaymax=10;
   /*END HARDCODE*/
  currenemies=nmy;
  for(i=0;i<nmy;i++)/*FIX: change to enemys*/
    {/*Loads enemies into battle*/
    
    random_enemy=(int)(((float)enemy_count)*rand()/(RAND_MAX+1.0));
    
    battle_enemies[i].hp=enemies[random_enemy].hp;
    battle_enemies[i].strength=enemies[random_enemy].strength;
    battle_enemies[i].agility=enemies[random_enemy].agility;
    battle_enemies[i].xp=enemies[random_enemy].xp;
    battle_enemies[i].offset_x=enemies[random_enemy].offset_x;
    battle_enemies[i].offset_y=enemies[random_enemy].offset_y;
    battle_enemies[i].dead=1;
    
    battle_enemies[i].image[0]=
                        IMG_Load(enemies[random_enemy].graphic_idle.c_str());
    battle_enemies[i].image[1]=
                        IMG_Load(enemies[random_enemy].graphic_attack.c_str());
    
    battle_enemies[i].attacking=0;
    if(battle_enemies[i].image[0]==NULL || battle_enemies[i].image[1] == NULL)
      {
      fprintf(stderr,"error loading image: %s",IMG_GetError());
      }
    /*FIX: enemy positions */
    battle_enemies[i].nmy_rect.x=0;
    battle_enemies[i].nmy_rect.y=0;
    battle_enemies[i].nmy_rect.w=NMYIMGWIDTH;
    battle_enemies[i].nmy_rect.h=NMYIMGHEIGHT;
    
    if(i==0)
      {
      battle_enemies[i].wrect.x=cell0.x;
      battle_enemies[i].wrect.y=cell0.y;
      }
    else if(i==1)
      {
      battle_enemies[i].wrect.x=cell1.x;
      battle_enemies[i].wrect.y=cell1.y;
      }
    else if(i==2)
      {
      battle_enemies[i].wrect.x=cell2.x;
      battle_enemies[i].wrect.y=cell2.y;
      }
    else if(i==3)
      {
      battle_enemies[i].wrect.x=cell3.x;
      battle_enemies[i].wrect.y=cell3.y;
      }
    
    }
  if(boss!=0)/*FIX for offset*/
    {/*loads a boss if there is one*/
    currenemies++;;
    battle_enemies[i].hp=::boss.hp;
    battle_enemies[i].strength=::boss.strength;
    battle_enemies[i].agility=::boss.agility;
    battle_enemies[i].xp=::boss.xp;
    battle_enemies[i].offset_x=::boss.offset_x;
    battle_enemies[i].offset_y=::boss.offset_y;
    battle_enemies[i].dead=1;
    battle_enemies[i].wrect.x=bosscell.x;
    battle_enemies[i].wrect.y=bosscell.y;
    battle_enemies[i].nmy_rect.h=NMYIMGHEIGHT-::boss.offset_x;
    battle_enemies[i].nmy_rect.w=NMYIMGWIDTH+::boss.offset_y;
    battle_enemies[i].image[0]=
                          IMG_Load(::boss.graphic_idle.c_str());
    battle_enemies[i].image[1]=
                          IMG_Load(::boss.graphic_attack.c_str());
    }
  cursor.image=IMG_Load("images/bcursor.png");
  cursor.selected=0;
  cursor.rect.w=CURSORW;
  cursor.rect.h=CURSORH;
  cursor.rect.x=battle_enemies[cursor.selected].wrect.x+
                (battle_enemies[cursor.selected].nmy_rect.w/2);
  cursor.rect.y=battle_enemies[cursor.selected].wrect.y-CURSORH;
  
  if(cursor.image==NULL)
    {
    fprintf(stderr,"error loading image: %s",IMG_GetError());
    }
    
  pcursor.image=IMG_Load("images/pcursor.png");
  pcursor.selected=0;
  pcursor.rect.w=CURSORW;
  pcursor.rect.h=CURSORH;
  pcursor.rect.x=battle_enemies[pcursor.selected].wrect.x+
                (battle_enemies[pcursor.selected].nmy_rect.w/2);
  pcursor.rect.y=battle_enemies[pcursor.selected].wrect.y-CURSORH;
  if(pcursor.image==NULL)
    {
    fprintf(stderr,"error loading image: %s",IMG_GetError());
    }
    
  }/*void battlefield_init()*/
  

void battle_destruct()
  {/*frees up images that are no longer needed*/
  int i;
  SDL_FreeSurface(battle.image);
  SDL_FreeSurface(battle.bmenu0);
  SDL_FreeSurface(battle.bmenu1);
  SDL_FreeSurface(cursor.image);
  SDL_FreeSurface(pcursor.image);

  for(i=0;i<MAXPLYRS;i++)
    {
    SDL_FreeSurface(battle_players[i].image[0]);
    SDL_FreeSurface(battle_players[i].image[1]);
    SDL_FreeSurface(battle_players[i].image[2]);
    }
  for(i=0;i<currenemies;i++)
    {
    SDL_FreeSurface(battle_enemies[i].image[0]);
    SDL_FreeSurface(battle_enemies[i].image[1]);
    }
  /*free battle fonts*/
  freeFont(&battlefont);
  freeFont(&highlight);
  freeFont(&heal);
  }/*void battle_destruct()*/


bool battleloop(int nmy, int boss)
  {/*the main loop true true survied battle false died*/
  Uint8* keys;
  SDL_Event event;
  int selected_menu_choice =0;
  int select_enemy = 1;
  int menu_level =0;
  int enemy_dead,enemy_turn;
  bool checkdead;
  int tmp_random;

  /*if no enemies are loaded, quit NOW!*/
  if(enemy_count == 0) return true;

  battlefield_init(nmy,boss);
  blitscreen(0,0);
  enemy_turn=12;
  updateDisplay();
  
  while((SDL_WaitEvent(&event)))
    {/*main even loop*/
    enemy_turn++;
    
    for(int go=0;go<nmy;go++)
      {
      if(enemy_turn%battle_enemies[go].agility==0 && battle_enemies[go].dead==1)
        {/*determines when normal enemies will go*/
        tmp_random=(int)(2.0*rand()/(RAND_MAX+1.0));
        printf("tmp_rand %d\n",tmp_random);
        eattacking(tmp_random, go ,selected_menu_choice);
        break;
        }
      }
      
    for(int go=0;go<boss;go++)
      {/*boss when normal enemies will go*/
      if(enemy_turn%battle_enemies[go].agility==0 && battle_enemies[go].dead==1)
        {
        tmp_random=(int)(2.0*rand()/(RAND_MAX+1.0));
        printf("tmp_rand %d\n",tmp_random);
        eattacking(tmp_random, go ,selected_menu_choice);
        break;
        }
      }  
    keys=SDL_GetKeyState(NULL);
    switch(event.type)
      {
      case SDL_QUIT: /*quits program*/
        exit(0);
        break;
      case SDL_KEYDOWN: /*registars key down*/
        if(keys[SDLK_DOWN])
          {
          selected_menu_choice++;
          }
        else if(keys[SDLK_LEFT])
          {
          if(select_enemy==0)
            {
            attack_cursor_l();
            }
          else
            {
            pcursor.selected--;
            if(pcursor.selected<0)
              {
              pcursor.selected=2;
              }
            if(status[pcursor.selected].status==DEAD)
              {
              for(int y=pcursor.selected;y>=0;y--)
               {
               if(status[y].status!=DEAD)
                  {
                  pcursor.selected=y;
                  break;
                  }
                }
              }
            if(status[pcursor.selected].status==DEAD)
              {
              for(int y=2;y>0;y--)
                {
                if(status[y].status!=DEAD)
                  {
                  pcursor.selected=y;
                  break;
                  }
                }
              }
            }
          }
        else if(keys[SDLK_RIGHT])
          {
          if(select_enemy==0)
            {
            attack_cursor_r();
            }
          else
            {
            pcursor.selected++;
            if(pcursor.selected>2)
              {
              pcursor.selected=0;
              }
            if(status[pcursor.selected].status==DEAD)
              {
              for(int y=pcursor.selected;y<MAXPLYRS;y++)
               {
                if(status[y].status!=DEAD)
                  {
                  pcursor.selected=y;
                  break;
                  }
                }
              }
            if(status[pcursor.selected].status==DEAD)
              {
              for(int y=0;y<MAXPLYRS;y++)
                {
                if(status[y].status!=DEAD)
                  {
                  pcursor.selected=y;
                  break;
                  }
                }
              }
            }
          }
        else if(keys[SDLK_UP])
          {
          selected_menu_choice--;
          }
        else if(keys[SDLK_RETURN])
          {
          if(selected_menu_choice==1 && menu_level==0)
            {
            menu_level=1;
            selected_menu_choice=0;
            }
          else if(select_enemy==0)
            {
            menu_level=pattacking(pcursor.selected, cursor.selected
                          ,selected_menu_choice, menu_level);
            select_enemy=1;
            }
          else
            {
            select_enemy=0;
            }
          }
        else if(keys[SDLK_SPACE])
          {/*FIX*/
          if(menu_level==1)
            {
            menu_level=0;
            }
          else
            {
            select_enemy=1;
            }
          }
        else if(keys[SDLK_ESCAPE])
          {
          battle_destruct();
          exit(0);/*REMOVE*/
          return true;
          }
        break;
      default: continue;
      }
    checkdead=blitscreen(select_enemy,selected_menu_choice);
    if(checkdead==false)
      {
      battle_destruct();
      return false;
      }
    
    battlemenu(menu_level);
    selected_menu_choice=bmenuselect(selected_menu_choice,menu_level);
    playerturn(pcursor.selected);
    updateDisplay();
    
    enemy_dead=0;
    
    for(int z=0; z<currenemies;z++)
      {
      if(battle_enemies[z].dead==0)
        {
        enemy_dead++;
        }
      }
    
    
    if(enemy_dead==currenemies)
      {/*giving exp and leveling really should be in it's own funciton*/
      int tmpexp;
      for(int p=0;p<currplayers;p++)
        {
        for(int b=0;b<currenemies;b++)
          {
          status[p].xp+=battle_enemies[b].xp;
          }
        }
      for(int p=0;p<currplayers;p++)
        {
        tmpexp=100-status[p].xp;
        if(tmpexp<0)
          {
          status[p].level++;
          status[p].xp=0;
          status[p].strength=(int)status[p].strength_mod;
          status[p].max_hp=(int)status[p].hp_mod;
          status[p].max_mp=(int)status[p].mp_mod;
          
          }
        }
      battle_destruct();
      
      return true;
      }
    enemy_dead=0;  
    for(int z=0; z<currplayers;z++)
      {/*checks if players are alive*/
      if(status[z].status==DEAD)
        {
        enemy_dead++;
        }
      }
    if(enemy_dead==MAXPLYRS)
      {
      return false;
      }
    }/*end while*/
  battle_destruct();
  return true;
  }/*void battleloop()*/


bool blitscreen(int select_enemy,int selected_menu_choice)
  {/*blits the main images to the screen excpet fighting peeople*/
  int i,tmpdead;
  SDL_Rect dest;
  tmpdead=0;
  SDL_BlitSurface(battle.image, NULL, screen, &battle.rect);
    for(i=0;i<currplayers;i++)/*FIX: change to current amount of plyrs*/
      {
      if(battle_players[i].attacking==1)
        {
        
        }
      else if(status[i].status!=DEAD)
        {
        SDL_BlitSurface(battle_players[i].image[0],NULL, screen, 
                        &battle_players[i].wrect);
        }
      else if(status[i].status==DEAD)
        {
        tmpdead++;
        }
      }
    if(tmpdead==3)
      {
      return false;
      }
    for(i=0;i<currenemies;i++)
      {
      if(battle_enemies[i].dead==0)
        {
        
        }
      else if(battle_enemies[i].dead==1 && battle_enemies[i].attacking==0)
        {/*
        printf("i %i dead %d attacking %d\n",i,battle_enemies[i].dead,
                battle_enemies[i].attacking);*/
        dest.x = battle_enemies[i].wrect.x + battle_enemies[i].offset_x;
        dest.y = battle_enemies[i].wrect.y + battle_enemies[i].offset_y;
        SDL_BlitSurface(battle_enemies[i].image[0],NULL, screen, 
                        &dest);
        }
      
      }
    if(select_enemy==0)
      {
      SDL_BlitSurface(cursor.image, NULL, screen, &cursor.rect);
      }

    delay(10, false);
    return true;
  }/*void blitscreen(int select_enemy,int selected_menu_choice)*/


int pattacking(int player,int enemy, int selected_menu_choice, int menu_level)
  {/*player attacks*/
  int i,changeframe;
  SDL_Rect tmp;
  char hit[DMGNUMLEN];
  int distancex, distancey;
  
  tmp.x=battle_players[player].wrect.x;
  tmp.y=battle_players[player].wrect.y;
  battle_players[player].attacking=1;
  
  distancex=((battle_players[player].wrect.x-(battle_enemies[enemy].wrect.x + 
              battle_enemies[enemy].nmy_rect.w))/ATTACKLOOP);
  distancey=(battle_players[player].wrect.y-battle_enemies[enemy].wrect.y)
            /ATTACKLOOP;
  if(selected_menu_choice==0 && menu_level==0)
    {
    for(i=0;i<ATTACKLOOP;i++)
      {/*move to enemy*/
      blitscreen(0, 0);
      SDL_BlitSurface(battle_players[player].image[0], NULL, 
                     screen, &tmp);
      updateDisplay();
      delay(10, false);
      tmp.x-=distancex;
      tmp.y-=distancey;
      }
    /*FIX attack animation at enemy*/
    
    blitscreen(0, 0);
    i=hitfor(status[player].strength);
    battle_enemies[enemy].hp-=i;
    sprintf(hit,"%d",i);  
    SDL_BlitSurface(battle_players[player].image[1], NULL, screen, &tmp);
    /*writes out dmg done to enemy*/
    writeFont(&highlight, screen,(battle_enemies[enemy].nmy_rect.w+
              battle_enemies[enemy].wrect.x), 
              battle_enemies[enemy].wrect.y, 
              hit);
    updateDisplay();
    delay(ATTACKDEL, false);
    
    for(i=0;i<ATTACKLOOP;i++)
      {/*move back*/
      blitscreen(0, 0);
      SDL_BlitSurface(battle_players[player].image[0], NULL, 
                      screen, &tmp);
      updateDisplay();
      delay(10, false);
      tmp.x+=distancex;
      tmp.y+=distancey;
      }
    }
  else if(menu_level==1 && status[player].mp>20)
    {/*casts spells if magic above 20*/
    distancex=((battle_players[player].wrect.x-battle_enemies[enemy].wrect.x) 
                  /MAGICLOOP);
    distancey=(battle_players[player].wrect.y-battle_enemies[enemy].wrect.y)
            /MAGICLOOP;
    changeframe=MAGICLOOP/MAGICFRAMES;
    
    for(i=0;i<MAGICLOOP;i++)
      {/*move to enemy*/
      blitscreen(0, 0);
      if(i%changeframe==0)
        {
        spell[selected_menu_choice].spell.x+=85;
        }
      SDL_BlitSurface(battle_players[player].image[2], NULL, 
                     screen, &battle_players[player].wrect);
      SDL_BlitSurface(spell[selected_menu_choice].magic, 
                      &spell[selected_menu_choice].spell, 
                      screen, 
                      &tmp);
      updateDisplay();
      delay(MAGICDEL, false);
      tmp.x-=distancex;
      tmp.y-=distancey;
      }
    i=hitfor(status[player].strength+status[player].level);
    battle_enemies[enemy].hp-=i;
    sprintf(hit,"%d",i);  
    status[player].mp-=20;
    sidebarUpdate();
    writeFont(&highlight, screen,(battle_enemies[enemy].nmy_rect.w+
              battle_enemies[enemy].wrect.x), 
              battle_enemies[enemy].wrect.y, 
              hit);
    updateDisplay();
    delay(MAGICDEL, false);
    spell[selected_menu_choice].spell.x=-85;
    }
  if(battle_enemies[enemy].hp<1)
    {
    battle_enemies[enemy].dead=0;
    attack_cursor_l();
    }
  
  battle_players[player].attacking=0;
  /*move cursor to new player*/
  /*
  pcursor.selected+=player;
  if(pcursor.selected<MAXPLYRS)
    {
    pcursor.selected=0;
    }
  if(status[pcursor.selected].status==DEAD)
      {*looking for that living player*
      for(int y=pcursor.selected;y<MAXPLYRS;y++)
        {
        if(status[y].status!=DEAD)
          {
          pcursor.selected=y;
          break;
          }
        }
      }
    if(status[pcursor.selected].status==DEAD)
      {*if it was still unable to find that living player*
      for(int y=0;y<MAXPLYRS;y++)
        {
        if(status[y].status!=DEAD)
          {
          pcursor.selected=y;
          break;
          }
        }
      }*/
  /*moves cursor to new player use in conjunction with wiat system when
  *available*/
  return 0;
  }/*int pattacking(int player,int enemy, int selected_menu_choice)*/

void battlemenu(int menu_level)
  {/*loads the battle menu*/
  int i;
  if(menu_level==0)
    {
    for(i=0;i<BMENUCHOICES;i++)
      {
      writeFont(&battlefont, battle.bmenu0, 3, i*BMENUOFFSET, bchoice0[i]);
      SDL_BlitSurface(battle.bmenu0, NULL, screen, &battle.bmenu_rect);
      }
    }
  else if(menu_level==1)
    {
    for(i=0;i<MAGICS;i++)
      {
      writeFont(&battlefont, battle.bmenu1, 3, i*BMENUOFFSET, bchoice1[i]);
      SDL_BlitSurface(battle.bmenu1, NULL, screen, &battle.bmenu_rect);
      }
    }
  }/*void battlemenu()*/


void playerturn(int selected)
  {/*set the pcursor[player cursor*/
  /*FIX check if player active*/
  if(pcursor.selected<0)
    {
    pcursor.selected=0;/*FIX: for appropiate amount of nmys*/
    }
  else if(pcursor.selected>(currplayers-1))
    {
    pcursor.selected=(currplayers-1);
    }
    
  pcursor.rect.x=battle_players[pcursor.selected].wrect.x+
               (battle_players[pcursor.selected].plyr_rect.w/2);
  pcursor.rect.y=battle_players[pcursor.selected].wrect.y-CURSORH;
  SDL_BlitSurface(pcursor.image, NULL, screen, &pcursor.rect);
  }/*void playerturn(int selected)*/


int bmenuselect(int selected, int menu_level)
  {
  if(selected<0)
    {
    selected=0;
    }
  else if(selected>(BMENUCHOICES-1) && menu_level==0)/*for zero based need -1*/
    {
    selected=BMENUCHOICES-1;/*for zero based need -1*/
    }
  else if(selected>(MAGICS-1) && menu_level==1)
    {
    selected=MAGICS-1;/*for zero based need -1*/
    }
  
  if(menu_level==0)
    {
    writeFont(&highlight, battle.bmenu0, 3, selected*BMENUOFFSET, 
             bchoice0[selected]);
    SDL_BlitSurface(battle.bmenu0, NULL, screen, &battle.bmenu_rect);
    }
  else if(menu_level==1)
    {
    
    writeFont(&highlight, battle.bmenu1, 3, selected*BMENUOFFSET, 
             bchoice1[selected]);
    SDL_BlitSurface(battle.bmenu1, NULL, screen, &battle.bmenu_rect);
    }
  return selected;
  }/*int bmenuselect(int selected)*/


void attack_cursor_l()
  {/*moves attack cursor to the left*/
  cursor.selected--;
  if(cursor.selected<0)
    {
    cursor.selected=currenemies-1;/*FIX: for appropiate amoutn of nmys*/
    }
  if(battle_enemies[cursor.selected].dead==0)
    {
    for(int i=currenemies;i>0;i--)
      {
      if(battle_enemies[i].dead==1)
        {
        cursor.selected=i;
        break;
        }
      }
    }
  cursor.rect.x=battle_enemies[cursor.selected].wrect.x+
               (battle_enemies[cursor.selected].nmy_rect.w/2);
  cursor.rect.y=battle_enemies[cursor.selected].wrect.y-CURSORH;
  }/*void attack_cursor_l()*/
  

void attack_cursor_r()
  {/*moves attack cursor to the right*/
  cursor.selected++;
  if(cursor.selected>=currenemies)/*FIX: amount of nmyies in battle*/
    {
    cursor.selected=0;
    }
  if(battle_enemies[cursor.selected].dead==0)
    {
    for(int i=0;i<currenemies;i++)
      {
      if(battle_enemies[i].dead==1)
        {
        cursor.selected=i;
        break;
        }
      }
    }
  cursor.rect.x=battle_enemies[cursor.selected].wrect.x+
                (battle_enemies[cursor.selected].nmy_rect.w/2);
  cursor.rect.y=battle_enemies[cursor.selected].wrect.y-CURSORH;
  }/*void attack_cursor_r()*/


void eattacking(int player,int enemy, int selected_menu_choice)
  {/*this enables the enemy to attack the player*/
  int i;
  SDL_Rect tmp, dest;
  char hit[DMGNUMLEN];
  int distancex, distancey;
  tmp.x=battle_enemies[enemy].wrect.x;
  tmp.y=battle_enemies[enemy].wrect.y;
  battle_enemies[enemy].attacking=1;
  
  if(status[player].status==DEAD)
    {
    for(int y=0;y<MAXPLYRS;y++)
      {
      if(status[y].status!=DEAD)
        {
        player=y;
        break;
        }
      }
    }
    
  distancex=(((battle_enemies[enemy].wrect.x+battle_enemies[enemy].nmy_rect.w)
            -battle_players[player].wrect.x)/ATTACKLOOP);
  distancey=(battle_enemies[enemy].wrect.y-battle_players[player].wrect.y)
            /ATTACKLOOP;

  for(i=0;i<ATTACKLOOP;i++)
    {/*move to player*/
    blitscreen(0, 0);
    dest.x = tmp.x + battle_enemies[enemy].offset_x;
    dest.y = tmp.y + battle_enemies[enemy].offset_y;
    SDL_BlitSurface(battle_enemies[enemy].image[0], NULL, 
                    screen, &dest);/*FIX: enemy images*/
    updateDisplay();
    delay(10, false);
    tmp.x-=distancex;
    tmp.y-=distancey;
    }
  /*FIX attack animation at player*/
  
  blitscreen(0, 0);
  
  i=hitfor(status[player].strength+status[player].level);
  status[player].hp-=i;
  sprintf(hit,"%d",i);  
  
  if(status[player].hp<1)
    {
    status[player].hp=0;
    status[player].status=DEAD;
    if(player>2)
      {/*if player dead puts players cursor on living player*/
      player=0;
      }
    if(status[player].status==DEAD)
      {/*looking for that living player*/
      for(int y=player;y<MAXPLYRS;y++)
        {
        if(status[y].status!=DEAD)
          {
          player=y;
          break;
          }
        }
      }
    if(status[player].status==DEAD)
      {/*if it was still unable to find that living player*/
      for(int y=0;y<MAXPLYRS;y++)
        {
        if(status[y].status!=DEAD)
          {
          player=y;
          break;
          }
        }
      }
    }

  dest.x = tmp.x + battle_enemies[enemy].offset_x;
  dest.y = tmp.y + battle_enemies[enemy].offset_y;
  SDL_BlitSurface(battle_enemies[enemy].image[1], NULL, screen, &dest);

  writeFont(&highlight, screen, battle_players[player].wrect.x, 
            battle_players[player].wrect.y, 
            hit);
  sidebarUpdate();
  updateDisplay();
  delay(ATTACKDEL, false);
  
  for(i=0;i<ATTACKLOOP;i++)
    {/*move back*/
    blitscreen(0, 0);
    dest.x = tmp.x + battle_enemies[enemy].offset_x;
    dest.y = tmp.y + battle_enemies[enemy].offset_y;
    SDL_BlitSurface(battle_enemies[enemy].image[0], NULL, 
                    screen, &dest);
    updateDisplay();
    delay(10, false);
    tmp.x+=distancex;
    tmp.y+=distancey;
    }
  
  battle_enemies[enemy].attacking=0;
  
  blitscreen(0, 0);
  battlemenu(0);
    selected_menu_choice=bmenuselect(selected_menu_choice,0);
    playerturn(pcursor.selected);
    updateDisplay();
  }/*void eattacking(int player,int enemy, int selected_menu_choice)*/


int hitfor(int a)
  {/*Ed's function to determine how much a person hits for*/
  int r1, r2;
  float poo;
  poo=a;
  r1 = (int)(poo*rand()/(RAND_MAX+1.0));
  r2 =(int) (poo*rand()/(RAND_MAX+1.0));
  return (int)((r1 + r2 + 1.5*a) /3);
  }/*int hitfor(int a)*/





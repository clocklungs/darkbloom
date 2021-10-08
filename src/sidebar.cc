#include <SDL/SDL.h>
#include "sidebar.hh"
#include "video.hh"
#include "game.hh"
#include "world.hh"

#define SBARLEFT 480
#define SBARBUFFERMAX 100
#define SBARTEXTMAX 9
#define SBARHEIGHT 141
#define SBARIMAGEH 92
#define SBARIMAGEW 60


VAR_paramList sidebar_vars;
Status status[7];


void sidebarUpdate(bool force)
{
  static Status local[3];
  static int localgold;
  static std::string localareaname;
  SDL_Surface *image;
  SDL_Rect src, dest;

  Font *font;
  char buffer[SBARBUFFERMAX];
  int changed;
  int i;

  /*whether or not to force the sidebar to update, even if there
  was no real change*/
  if(force) changed = 1;
  else changed = 0;

  if(localgold != gold)
  {
    localgold = gold;
    changed++;
  }

  if(localareaname != world.name)
  {
    localareaname = world.name;
    changed++;
  }

/*this was here in case i wanted to make the update more aware of
what had changed, as well as perhaps adding per change effects
like playing sounds on status changes*/
  for(i=0; i<3; ++i)
  {
    if(strcmp(local[i].name, status[i].name))
    {
      strcpy(local[i].name, status[i].name);
      changed++;
    }
    if(strcmp(local[i].Class, status[i].Class))
    {
      strcpy(local[i].Class, status[i].Class);
      changed++;
    }
    if(strcmp(local[i].weapon, status[i].weapon))
    {
      strcpy(local[i].weapon, status[i].weapon);
      changed++;
    }
    if(local[i].level != status[i].level)
    {
      local[i].level = status[i].level;
      changed++;
    }
    if(local[i].hp != status[i].hp)
    {
      local[i].hp = status[i].hp;
      changed++;
    }
    if(local[i].max_hp != status[i].max_hp)
    {
      local[i].max_hp = status[i].max_hp;
      changed++;
    }
    if(local[i].mp != status[i].mp)
    {
      local[i].mp = status[i].mp;
      changed++;
    }
    if(local[i].max_mp != status[i].max_mp)
    {
      local[i].max_mp = status[i].max_mp;
      changed++;
    }
    if(local[i].status != status[i].status)
    {
      local[i].status = status[i].status;
      changed++;
    }
    if(local[i].active != status[i].active)
    {
      local[i].active = status[i].active;
      changed++;
    }
  }

  /*don't update if nothing is new*/
  if(changed == 0) return;

  /*make sure sidebar image has been loaded*/
  if(sideBar == NULL)
  {
    fprintf(stderr, "sideBar surface uninitialized\n");
    return;
  }

  /*set font*/
  font = &sideBarFont;

  /*draw sidebar background*/
  dest.x = SBARLEFT;
  dest.y = 0;
  SDL_BlitSurface(sideBar, NULL, screen, &dest);

  /*draw up to three side bar people*/
  for(i=0; i<3; ++i)
  {
    /*stop updating if we hit an inactive player*/
    if(status[i].active == false) break;

    /*set offsets*/
    dest.x = SBARLEFT + 3;
    dest.y = 3 + i*SBARHEIGHT;

    /*draw player name*/
    snprintf(buffer, SBARTEXTMAX, "%s", status[i].name);
    buffer[SBARTEXTMAX] = '\0';
    writeFont(font, screen, dest.x + 61, dest.y, status[i].name);
    /*draw level stat*/
    snprintf(buffer, SBARTEXTMAX, "LVL: %3.i", status[i].level);
    buffer[SBARTEXTMAX] = '\0';
    writeFont(font, screen, dest.x + 61, dest.y+font->h, buffer);
    /*draw player class*/
    snprintf(buffer, SBARTEXTMAX, "%s", status[i].Class);
    buffer[SBARTEXTMAX] = '\0';
    writeFont(font, screen, dest.x + 61, dest.y+font->h*2, buffer);

    /*draw hp stats*/
    snprintf(buffer, SBARTEXTMAX+2, "%.3i/%.3i HP", status[i].hp,
             status[i].max_hp);
    buffer[SBARTEXTMAX+2] = '\0';
    rightFont(font, screen, screen->w-3, dest.y+SBARIMAGEH, buffer);
    /*draw mp stats*/
    
    snprintf(buffer, SBARTEXTMAX+2, "%.3i/%.3i MP", status[i].mp,
             status[i].max_mp);
    buffer[SBARTEXTMAX+2] = '\0';
    rightFont(font, screen, screen->w-3, dest.y+SBARIMAGEH+font->h, buffer);

    /*load player sidebar image, if any*/
    if(sidebar_vars.toString(status[i].name, buffer, SBARBUFFERMAX) != NULL)
    {
      image = IMG_Load(buffer);
      if(image == NULL)
      {
        fprintf(stderr, "unable to load sidebar picture: %s\n", IMG_GetError());
      }
      else
      {
        /*bound the player image to make sure it fits*/
        src.x = 0;
        src.y = 0;
        src.w = SBARIMAGEW;
        src.h = SBARIMAGEH;
        /*draw player image*/
        SDL_BlitSurface(image, &src, screen, &dest);
        /*free surface memory*/
        SDL_FreeSurface(image);
        image = NULL;
        if(status[i].status == POISONED)
        {
          image = IMG_Load("images/poisoned_effect.png");
          if(image != NULL)
          {
            SDL_BlitSurface(image, &src, screen, &dest);
            SDL_FreeSurface(image);
            image = NULL;
          }
          else
          {
            fprintf(stderr, "error: %s\n", IMG_GetError());
          }
        }
      }
    }

    /*set status font, draw status*/
    font = &sideBarStatusFont;
    switch(status[i].status)
    {
      case NORMAL:
        break;
      case CRITICAL:
        writeFont(font, screen, dest.x + 61, dest.y+2+font->h*3, "CRITICAL");
        break;
      case POISONED:
        writeFont(font, screen, dest.x + 61, dest.y+2+font->h*3, "POISONED");
        break;
      case DEAD:
        writeFont(font, screen, dest.x + 61, dest.y+2+font->h*3, "DEAD");
        break;
    }
    /*set font back to normal*/
    font = &sideBarFont;

    /*check if they have a weapon equipped*/
    if(strlen(status[i].weapon) &&
       sidebar_vars.toString(status[i].weapon, buffer, SBARBUFFERMAX) != NULL)
    {
      image = IMG_Load(buffer);
      if(image == NULL)
      {
        fprintf(stderr, "unable to load weapon image: %s\n", IMG_GetError());
      }
      else
      {
        dest.x = SBARLEFT + 3;
        dest.y = 4 + SBARIMAGEH + i*SBARHEIGHT;
        /*bound the weapon image, to make sure it fits*/
        src.x = 0;
        src.y = 0;
        src.w = 45;
        src.h = 45;
        SDL_BlitSurface(image, &src, screen, &dest);
        /*free surface memory*/
        SDL_FreeSurface(image);
        image = NULL;
      }
    }
  }
  dest.x = SBARLEFT + 3;
  dest.y = 5 + 3*SBARHEIGHT;

  /*draw gold*/
  snprintf(buffer, SBARTEXTMAX+7, "GOLD: %9.i ", gold);
  buffer[SBARTEXTMAX+7] = '\0';
  writeFont(font, screen, dest.x, dest.y, buffer);
  /*display area name*/
  snprintf(buffer, SBARTEXTMAX+7, "%s", world.name.c_str());
  buffer[SBARTEXTMAX+7] = '\0';
  rightFont(font, screen, screen->w-3, dest.y+font->h, buffer);

  printf("SIDEBAR UPDATE\n");

}

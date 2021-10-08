#include <stdio.h>
#include <time.h>
#include "game.hh"
#include "video.hh"
#include "sound.hh"
#include "button.hh"
#include "dbox.hh"
#include "dialog.hh"
#include "world.hh"
#include "plyr.hh"
#include "sidebar.hh"
#include "object.hh"
#include "pathfinding.hh"
#include "script.hh"
#include "battle.hh"
#include "enemy.hh"


void initGraphics()
{
  /*load fonts
  this section will probably be heavily revised when were ready for release*/
  initFont(&font,              "images/font.png");
  initFont(&menuFont,          "images/menufont.png");
  initFont(&menuHighlightFont, "images/menufonthighlight.png");
  initFont(&dboxFont,          "images/dboxfont.png");
  initFont(&dboxHighlightFont, "images/dboxfonthighlight.png");
  initFont(&sideBarFont,       "images/sidebarfont.png");
  initFont(&sideBarStatusFont, "images/sidebarstatusfont.png");

  /*load menu background image*/
  menuBackground = IMG_Load("images/menubackground.png");
  if(menuBackground == NULL)
  {
    fprintf(stderr, "unable to load menu image: %s\n", IMG_GetError());
    exit(1);
  }

  /*load dialog box image*/
  dbox = IMG_Load("images/dialogbox.png");
  if(dbox == NULL)
  {
    fprintf(stderr, "unable to initialize dialog box: %s\n", IMG_GetError());
    exit(1);
  }

  /*load sidebar image*/
  sideBar = IMG_Load("images/sidebar.png");
  if(sideBar == NULL)
  {
    fprintf(stderr, "unable to initialize sidebar: %s\n", IMG_GetError());
    exit(1);
  }
  srand(time(NULL));/*seed random*/
}


void startGame()
{
  /*start fresh*/
  dialog_vars.clear();
  script_vars.clear();
  sidebar_vars.clear();
  objects.clear();
  /*need to work on a better stat system*/
  stats_init();
  game_started = true;
  gameLoop();
}


void continueGame()
{
  /*can't continue if we haven't started*/
  if(!game_started) return;
  gameLoop();
}


void gameLoop()
{
  msecs_per_frame = MSECS_PER_FRAME; //temporary
  worldLoop();
  gameOver();
}


void gameOver()
{
  printf("STUB: GAME OVER\n");
}


void worldLoop()
{
  SDL_Event event;
  Uint32 ticks, oldticks;
  bool goodevent;
  int dest_x, dest_y;
  ObjectLayerList visible_objects;
  Object *collide_object, *active_object;

  collide_object = NULL;
  active_object  = NULL;

  world.load("maps/start.map");

  player_init();
  /*set player starting tile, this should probably be a call to a script*/
  player.x = 1;
  player.y = 1;

  /*adjust world viewable to accomodate player position*/
  world.follow(player);
  world.draw();

  /*draw player and any world objects*/
  visible_objects.add(objects);
  visible_objects.add(player);
  visible_objects.draw();

  /*force an update of the sideBar*/
  sidebarUpdate(true);

  updateDisplay();

  // TODO: Update controls to just check key state instead of relying on key repeat
  //SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  oldticks = SDL_GetTicks();

  flushEvents();

  while(true)
  {
    /*update how long it has been since last frame update*/
    oldticks = SDL_GetTicks();

    /*draw frame*/
    /*follow the player*/
    world.follow(player);
    /*draw world*/
    world.draw();
    /*draw player and objects*/
    visible_objects.add(objects);
    visible_objects.add(player);
    visible_objects.draw();
    /*update sidebar, as necessary*/
    sidebarUpdate();
    /*update screen*/
    updateDisplay();

    if(collide_object != NULL)
    {
      //printf("WE COLLIDED W/ %s!\n", collide_object->name.c_str());
      if(!runScript(player, *(collide_object), COLLISION)) return;
      collide_object = NULL;
    }

    goodevent = flushJunkEvents(&event);
    ticks = SDL_GetTicks();
    while((ticks-oldticks) < msecs_per_frame)
    {
      SDL_Delay(10);
      ticks = SDL_GetTicks();
    }

    //printf("DEBUG: elapsed: %i\n", ticks-oldticks);

    if(goodevent)
    {/*main even loop*/
      switch(event.type)
      {
        /*keydown events can repeat, be careful what you put here*/
        // TODO: switch to SDL_GetKeyboardState and check the returned "keys" state
        case SDL_KEYDOWN:
          switch(event.key.keysym.scancode)
          {
            case SDL_SCANCODE_DOWN:
              collide_object = player.down();
              break;
            case SDL_SCANCODE_UP:
              collide_object = player.up();
              break;
            case SDL_SCANCODE_LEFT:
              collide_object = player.left();
              break;
            case SDL_SCANCODE_RIGHT:
              collide_object = player.right();
              break;
            default:
              break;
          }
          break;
        /*these will only happen once per keypress->keyrelease*/
        case SDL_KEYUP:
          switch(event.key.keysym.scancode)
          {
            case SDL_SCANCODE_ESCAPE:
              return;
            case SDL_SCANCODE_SPACE:
              /*start a boss battle.
              this is here for debugging purposes,
              SPACE should probably be used as another 'activate' button*/
              load_boss("enemies/giant_rat.boss");
              battleloop(2,1);
              break;
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_KP_ENTER:
              active_object = player.activate();
              if(active_object != NULL)
              {
                //printf("WE ACTIVATED %s!\n", active_object->name.c_str());
                if(!runScript(player, *(active_object), ACTIVATION)) return;
                active_object = NULL;
              }
              break;
            default:
              checkToggles(event.key.keysym.scancode);
              break;
          }
          break;
        case SDL_MOUSEBUTTONDOWN:
          /*check if the click is in the game area*/
          if(event.button.x > 480 || event.button.y > 480) break;
          /*it was, convert the button click to a tile x, y*/
          dest_x = (event.button.x+(world.viewable.x - world.viewport.x))
                   / TILEWIDTH;
          dest_y = (event.button.y+(world.viewable.y - world.viewport.y))
                   / TILEHEIGHT;
          /*clear old stack*/
          player.move_stack.empty();
          /*path find to it*/
          path_find(player, dest_x, dest_y);
          if(event.button.button == SDL_BUTTON_RIGHT) player.setSpeed(RUN);
          else player.setSpeed(WALK);
          break;
        default:
          break;
      }
    }
  }
}

/*EOF*/

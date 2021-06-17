#include "video.hh"
#include "sound.hh"
#include "dbox.hh"


/*video information/settings*/
struct {
  int w;             /*width*/
  int h;             /*height*/
  int bpp;           /*bits per pixel (0 = desktop bpp)*/
  Uint32 sdl_flags;  /*video flags for sdl-only mode*/
  } video;


/*initialize SDL and display*/
void initSDL()
{
  /*initialize SDL video/timer subsystems*/
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO)<0)
  {
    fprintf(stderr, "unable to init SDL: %s\n", SDL_GetError());
    exit(1);
  }
  /*shutdown SDL on program termination*/
  atexit(SDL_Quit);

  /*save configuration on exit*/
  atexit(saveConfig);
}


void initVideo(const char *title)
{
  /*set initial resolution width/height/bits per pixel*/
  /*these are pretty save defaults*/
  video.w         = 640;
  video.h         = 480;
  video.bpp       = 0;

  /*set video _flags for 2d and 3d (openGL) modes*/
  video.sdl_flags=SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_ANYFORMAT;

#ifdef WIN32
  /*attempt to work around fullscreen bug under windows*/
  if(config.fullscreen)
  {
    video.sdl_flags &= ~SDL_DOUBLEBUF;
  }
#endif

  /*change to initial mode*/
  changeModeSDL();
  SDL_WM_SetCaption(title, title);

  /*initialize dialogbox backup surface*/
  dbox_back = SDL_CreateRGBSurface(screen->flags, dbox->w, dbox->h,
                                   screen->format->BitsPerPixel,
                                   screen->format->Rmask,
                                   screen->format->Gmask,
                                   screen->format->Bmask,
                                   screen->format->Amask);
  if(dbox_back == NULL)
  {
    fprintf(stderr, "unable to initialize dialog box: %s\n", SDL_GetError());
    exit(1);
  }
}


void changeModeSDL()
{
  /*set video mode to w x _h x bpp (with fullscreen toggle checked)*/
  screen=SDL_SetVideoMode(video.w, video.h, video.bpp, config.fullscreen?
                          video.sdl_flags|SDL_FULLSCREEN:video.sdl_flags);
  if(screen==NULL)
  {
    fprintf(stderr, "unable to set %ix%i video mode: %s\n", video.w, video.h,
            SDL_GetError());
    exit(1);
  }
}


void toggleFullscreen()
{
  SDL_Surface *save;

  /*create surface to save the screen's content*/
  save = SDL_CreateRGBSurface(screen->flags, screen->w, screen->h,
                              screen->format->BitsPerPixel,
                              screen->format->Rmask, screen->format->Gmask,
                              screen->format->Bmask, screen->format->Amask);
  if(save == NULL)
  {
    fprintf(stderr, "unable to create surface: %s\n", SDL_GetError());
    exit(1);
  }
  /*save the screen's content*/
  SDL_BlitSurface(screen, NULL, save, NULL);

  /*toggle fullscreen flag*/
  config.fullscreen = !config.fullscreen;
#ifdef WIN32
  /*attempt to work around fullscreen bug under windows*/
  if(config.fullscreen)
  {
    /*disable double buffering*/
    video.sdl_flags &= ~SDL_DOUBLEBUF;
  }
  else
  {
    /*enable double buffering*/
    video.sdl_flags |= SDL_DOUBLEBUF;
  }
#endif
  /*switch mode to/from fullscreen*/
  changeModeSDL();
  /*restore screen contents*/
  SDL_BlitSurface(save, NULL, screen, NULL);
  /*update*/
  updateDisplay();
  /*free save surface memory*/
  SDL_FreeSurface(save);
}


void flushEvents()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT) exit(1);
  }
}


/********************************************************************
bool flushJunkEvents(SDL_Event *event)
  TAKES: an SDL_Event structure
  runs through the current event queue, returning immediately if it
  encounters a non-junk event (keyboard event, mousebutton event).
  if it encounters an SDL_QUIT event, it terminates the program
  RETURNS: a boolean indicating the presence of a non-junk event.
********************************************************************/
bool flushJunkEvents(SDL_Event *event)
{
  /*a non-existant event structure would be bad*/
  if(event == NULL)
  {
    fprintf(stderr, "flushJunkEvents: NULL event structure pointer\n");
    exit(-1);
  }

  /*run through the event queue*/
  while(SDL_PollEvent(event))
  {
    switch(event->type)
    {
      case SDL_QUIT:
        exit(1);
      case SDL_KEYUP:
      case SDL_KEYDOWN:
      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN:
        return true;
      default:
        continue;
    }
  }
  return false;
}


/*maybe i should rewrite this to use SDL_GetTicks()
but this way is less cpu intensive*/
void delay(unsigned int ms, bool escapeable)
{
  SDL_Event event;
  unsigned int msecs;

  msecs = 0;

  while(msecs < ms)
  {
    msecs += 10;
    SDL_Delay(10);
    if(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT) exit(1);
      if(escapeable)
      {
        if(event.type == SDL_MOUSEBUTTONUP) return;
        if(event.type == SDL_KEYUP)
        {
          switch(event.key.keysym.sym)
          {
            case SDLK_ESCAPE:
            case SDLK_RETURN:
            case SDLK_SPACE:
            case SDLK_KP_ENTER:
              return;
            default:
              /*check for sound/music/fullscreen toggle*/
              checkToggles(event.key.keysym);
              continue;
          }
        }
      }
    }
  }
}


void updateDisplay()
{
#ifdef WIN32
  /*we don't use double buffer for fullscreen under win32*/
  if(config.fullscreen)
  {
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    return;
  }
#endif
  SDL_Flip(screen);
}


/*check the sounds/music/fullscreen toggle buttons*/
int checkToggles(SDL_keysym key)
{
  switch(key.sym)
  {
    case SDLK_F2:
      toggleSounds();
      return 1;
    case SDLK_F3:
      toggleMusic();
      return 2;
    case SDLK_F11:
      toggleFullscreen();
      return 3;
    default:
      return 0;
  }
}

/*EOF*/

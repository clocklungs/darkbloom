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


SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *screenTexture;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const Uint32 rmask = 0xff000000;
    const Uint32 gmask = 0x00ff0000;
    const Uint32 bmask = 0x0000ff00;
    const Uint32 amask = 0x000000ff;
#else
    const Uint32 rmask = 0x000000ff;
    const Uint32 gmask = 0x0000ff00;
    const Uint32 bmask = 0x00ff0000;
    const Uint32 amask = 0xff000000;
#endif

SDL_Surface *create_rgba_surface(const unsigned int width, const unsigned int height) {
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
    if (surface == NULL) {
        fprintf(stderr, "SDL_CreateRGBSurface failed: %s\n", SDL_GetError());
        exit(3);
    }
    return surface;
}

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
  video.sdl_flags=0; //SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_ANYFORMAT;

#ifdef WIN32
  /*attempt to work around fullscreen bug under windows*/
  if(config.fullscreen)
  {
    video.sdl_flags &= ~SDL_DOUBLEBUF;
  }
#endif

  /*change to initial mode*/
  window = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    1024, 768,
    SDL_WINDOW_RESIZABLE
  );
  if (window == NULL) {
    fprintf(stderr, "Error creating game window: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    fprintf(stderr, "Error creating game renderer: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
  SDL_RenderSetLogicalSize(renderer, 640, 480); // TODO: use video.w/.h?

  screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 640, 480);
  screen = create_rgba_surface(640, 480);

  /*initialize dialogbox backup surface*/
  // TODO: use create_rgba_surface()
  dbox_back = SDL_CreateRGBSurface(screen->flags, dbox->w, dbox->h,
                                   screen->format->BitsPerPixel,
                                   screen->format->Rmask,
                                   screen->format->Gmask,
                                   screen->format->Bmask,
                                   screen->format->Amask);
  if(dbox_back == NULL) {
    fprintf(stderr, "unable to initialize dialog box: %s\n", SDL_GetError());
    exit(1);
  }
}

void toggleFullscreen()
{
  // TODO: Is this screen saving stuff needed?
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
  SDL_SetWindowFullscreen(window, config.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

  /*restore screen contents*/
  SDL_BlitSurface(save, NULL, screen, NULL);
  /*free save surface memory*/
  SDL_FreeSurface(save);

  /*update*/
  updateDisplay();
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
              checkToggles(event.key.keysym.scancode);
              continue;
          }
        }
      }
    }
  }
}


void updateDisplay()
{
  SDL_UpdateTexture(screenTexture, NULL, screen->pixels, screen->pitch);
  SDL_RenderClear(renderer); // clear
  SDL_RenderCopy(renderer, screenTexture, NULL, NULL); // render the whole screenTexture
  SDL_RenderPresent(renderer); // update the display hardware
}


/*check the sounds/music/fullscreen toggle buttons*/
int checkToggles(const SDL_Scancode scancode)
{
  switch(scancode)
  {
    case SDL_SCANCODE_F2:
      toggleSounds();
      return 1;
    case SDL_SCANCODE_F3:
      toggleMusic();
      return 2;
    case SDL_SCANCODE_F11:
      toggleFullscreen();
      return 3;
    default:
      return 0;
  }
}

/*EOF*/

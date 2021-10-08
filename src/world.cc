#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "SDL.h"
#include "SDL_image.h"

#include "world.hh"
#include "sidebar.hh"
#include "object.hh"
#include "plyr.hh"
#include "enemy.hh"

#define NAMELENGTH 100 /*string length for graphic and directory names*/


extern SDL_Surface *screen; /*display surface*/


/*********************************************************************
World constructor
  intializes the SDL_rects that correspond to the portion of the screen
  where the map will be displayed (viewport) and the portion of
  the map that will be displayed (viewable)
*********************************************************************/
World::World()
{
  /*mark the image data as empty*/
  image = NULL;
  /*set where the map will display on the screen*/
  viewport.x = 0;
  viewport.y = 0;
  viewport.w = TILEWIDTH  * WWDISPLAY;
  viewport.h = TILEHEIGHT * WHDISPLAY;
  /*set the maps viewable area*/
  viewable.x = 0;
  viewable.y = 0;
  viewable.w = TILEWIDTH  * WWDISPLAY;
  viewable.h = TILEHEIGHT * WHDISPLAY;
  /*set the width and height*/
  w = 0;
  h = 0;
  /*set the name*/
  name = "";
}

/*********************************************************************
World destructor
  just free the image data, if any
*********************************************************************/
World::~World()
{
  if(image != NULL)
  {
    SDL_FreeSurface(image);
    image = NULL;
  }
}


/*********************************************************************
World draw()
  blit the viewable world image data to the viewport location of the
  display (screen)
*********************************************************************/
void World::draw()
{
  SDL_Rect src, dest;

  if(image == NULL)
  {
    fprintf(stderr, "cannot draw world: no image data exists\n");
    return;
  }

  /*copy the viewable and viewport structs, to avoid them being
  changed by SDL_BlitSurface*/
  src.x = viewable.x;
  src.y = viewable.y;
  src.w = viewable.w;
  src.h = viewable.h;

  dest.x = viewport.x;
  dest.y = viewport.y;
  dest.w = viewport.w;
  dest.h = viewport.h;

  /*draw the viewable world image data to the viewport location*/
  SDL_BlitSurface(image, &src, screen, &dest);
}


/*********************************************************************
World createImage()
  allocate memory for the 'image' surface (where all the map image
  data is stored) and populate it based on the contents of the world
  array (that is, load all the tiles, blit them to the proper
  locations in the world image, and then free them
*********************************************************************/
void World::createImage(const char *filename)
{
  fprintf(stdout, "Creating world image for given tiles %s\n", filename);
  SDL_Surface *tiles[255];
  SDL_Rect dest;
  int i, x, y;
  int line;
  char buffer[100];
  char c;
  FILE *in;

  /*this should be unnecessary, just being safe*/
  for(i=0; i<255; ++i) if(tiles[i] != NULL) tiles[i] = NULL;

  /*free old surface before allocating new one, (may be different size)*/
  if(image != NULL)
  {
    SDL_FreeSurface(image);
    image = NULL;
  }

  /*open tilelist (in text mode) for reading*/
  in = fopen(filename, "rt");
  if(in == NULL)
  {
    fprintf(stderr, "error opening tilelist\n");
    perror(filename);
    exit(1);
  }

  line = 1;
  while(!feof(in))
  {
    /*remove leading spaces/zeros from tilenumber specification*/
    do
    {
      c = fgetc(in);
      if(c == EOF)
      {
        break;
      }
      if(c == '\n') line++;
      if(c != '0' && !isspace(c))
      {
        if(ungetc(c, in) == EOF)
        {
          fprintf(stderr, "error putting char back into stream: %c\n", c);
          exit(1);
        }
      }
    } while(c == '0' || isspace(c));

    /*we hit the end of the file, break the loop*/
    if(c == EOF) break;

    /*read in tile number and tile filename*/
    if(fscanf(in, "%d %s99\n", &x, buffer) != 2)
    {
      fprintf(stderr, "malformed tilefile: %s: %i\n", filename, line);
      break;
    }

    if(x > 255)
    {
      fprintf(stderr, "warning: %s: %i\n", filename, line);
      fprintf(stderr, "tile number greater than 255: %i\n", x);
    }

    /*load the tile image*/
    i = (unsigned char)x;
    if(tiles[i] != NULL)
    {
      fprintf(stderr, "warning: %s: %i\n", filename, line);
      fprintf(stderr, "redefinition of tile %i\n", i);
      SDL_FreeSurface(tiles[i]);
    }
    tiles[i] = IMG_Load(buffer);
    if(tiles[i] == NULL)
    {
      fprintf(stderr, "%s: %i : %s\n", filename, line, IMG_GetError());
    }
  }
  fclose(in);

  /*allocate a surface large enough to hold all the map image data*/
  image = SDL_CreateRGBSurface(SDL_SWSURFACE,
                               w  * TILEWIDTH,
                               h * TILEHEIGHT,
                               screen->format->BitsPerPixel,
                               screen->format->Rmask,
                               screen->format->Gmask,
                               screen->format->Bmask,
                               screen->format->Amask);
  if(image == NULL)
  {
    fprintf(stderr, "SDL_CreateRGBSurface failed: %s\n", SDL_GetError());
    exit(1);
  }

  /*for each row*/
  for(y=0; y<h; y++)
  {
    /*for each column*/
    for(x=0; x<w; x++)
    {
      /*set i to value of current cell/tile*/
      i = world[y][x];
      /*check if tile hasn't been loaded*/
      if(tiles[i] == NULL)
      {
        fprintf(stderr, "missing graphic definition for tile %i\n", i);
        continue;
      }
      /*set tile destination coordinates*/
      dest.x = x*TILEWIDTH;
      dest.y = y*TILEHEIGHT;
      /*draw tile image to image*/
      SDL_BlitSurface(tiles[i], NULL, image, &dest);
    }
  }

  /*clean up after ourselves/free the loaded tile memory*/
  for(i=0; i<255; ++i)
  {
    if(tiles[i] != NULL)
    {
      SDL_FreeSurface(tiles[i]);
      tiles[i] = NULL;
    }
  }
} /*ENDOF set_screen(int width, int height)*/


void World::load(const char *filename)
{
  fprintf(stdout, "Loading %s\n", filename);
  FILE *in;
  char areaname[NAMELENGTH];
  std::string tilelist;
  char buffer[100];
  int i;
  int x, y; /*number from file*/
  int wx, wy; /*level Height, Width*/
  char c;
  int line;
  int objects;
  Object *object;

  line = 1;

  /*open level file (in text mode) for reading*/
  in = fopen(filename, "rt");
  if(in == NULL)
  {
    fprintf(stderr, "error opening level file\n");
    perror(filename);
    exit(1);
  }

  /*initialises game world array to empty*/
  for(wy=0; wy<WORLDHEIGHTMAX; wy++)
  {
    for(wx=0; wx<WORLDWIDTHMAX; wx++)
    {
      world[wy][wx] = WORLDEMPTY;
    }
  }

  if(fgets(areaname, NAMELENGTH, in) == NULL)
  {
    fprintf(stderr, "malformed level file: %s: %i\n", filename, line);
    fprintf(stderr, "file does not start w/ area name\n");
    exit(1);
  }
  /*strip EOL character from areaname*/
  areaname[strlen(areaname)-1] = '\0';

  /*set global areaname*/
  name = areaname;
  line++;
  /*clear any non persistent objects*/
  ::objects.flush();
  /*reactivate any persistent objects that belong to this area*/
  ::objects.activate();


  do
  {
    /*get enemylist filename*/
    if(fgets(buffer, 100, in) == NULL)
    {
      fprintf(stderr, "malformed level file: %s: %i\n", filename, line);
      fprintf(stderr, "missing enemylist\n");
      fclose(in);
      return;
    }
    line++;
    /*strip trailing newline*/
    i = strlen(buffer);
    if(i) buffer[--i] = '\0';
  } while (!i);

  /*if an enemy list was specified (i.e, they didn't write NONE, load enemies*/
  if(strcmp(buffer, "NONE")) load_enemies(buffer);

  do
  {
    /*get tilelist filename*/
    if(fgets(buffer, 100, in) == NULL)
    {
      fprintf(stderr, "malformed level file: %s: %i\n", filename, line);
      fprintf(stderr, "missing tilelist\n");
      fclose(in);
      return;
    }
    line++;
    /*strip trailing newline*/
    i = strlen(buffer);
    if(i) buffer[--i] = '\0';
  } while (!i);

  /*save tilelist*/
  tilelist = buffer;

  /*get level width and height, and make sure they are within bounds*/
  if((fscanf(in, "%d%d", &w, &h) < 2) ||
     (w < WWDISPLAY || h < WHDISPLAY) ||
     (w > WORLDWIDTHMAX || h > WORLDHEIGHTMAX))
  {
    fprintf(stderr, "malformed level file: %s: %i\n", filename, line);
    fprintf(stderr, "bad width/height specification\n");
    exit(1);
  }

  /*read in tile data*/
  for(wy=0; wy<h; wy++)
  {
    for(wx=0; wx<w; wx++)
    {/*creates game world from file*/
      do
      {
        c = fgetc(in);
        if(c == '\n')
        {
          if(wx != 0)
          {
            /*this won't catch all of these, but still better than nothing*/
            fprintf(stderr, "warning: %s: %i\n", filename, line);
            fprintf(stderr, "newline before end of row\n");
          }
          line++;
        }
        if(c == EOF)
        {
          fprintf(stderr, "malformed level file: %s: %i\n", filename, line);
          fprintf(stderr, "too few tiles specified\n");
          exit(1);
        }
        if(c != '0' && !isspace(c))
        {
          if(ungetc(c, in) == EOF)
          {
            fprintf(stderr, "error putting char back into stream: %c\n", c);
            exit(1);
          }
        }
      } while(c == '0');

      if(fscanf(in, "%d", &x) == 0)
      {
        fprintf(stderr, "malformed level file: %s: %i\n", filename, line);
        fprintf(stderr, "too few tiles specified\n");
        exit(1);
      }

      if(x > 255)
      {
        fprintf(stderr, "warning: %s: %i\n", filename, line);
        fprintf(stderr, "tile number greater than 255: %i\n", x);
      }

      /*set tile value in world array*/
      world[wy][wx] = (unsigned char)x;
    }
  }

  /*load all map objects*/
  if(fscanf(in, "%i", &objects) == 0)
  {
    fprintf(stderr, "warning: %s:", filename);
    fprintf(stderr, "missing object count\n");
  }
  else
  {
    for(i=0; i<objects; ++i)
    {
      /*get object coordinates*/
      if(fscanf(in, "%i %i\n", &x, &y) < 2)
      {
        fprintf(stderr, "error: %s: ", filename);
        fprintf(stderr, "malformed object coordinate specification\n");
        break;
      }
      /*get object filename*/
      if(fscanf(in, "%99s\n", buffer) == 0)
      {
        fprintf(stderr, "error: %s: ", filename);
        fprintf(stderr, "malformed object specification\n");
        break;
      }
      /*check if object is within world bounds*/
      if(x < 0 || x >= w || y < 0 || y >= h)
      {
        fprintf(stderr, "warning: %s: ", filename);
        fprintf(stderr, "object '%s' outside of world bounds\n", buffer);
        continue;
      }
      /*load object*/
      object = ::objects.add(buffer, y);
      if(object == NULL)
      {
        fprintf(stderr, "error: %s: ", filename);
        fprintf(stderr, "invalid object: %s\n", buffer);
        break;
      }
      /*set object coords*/
      object->x = x;
      object->y = y;
    }
  }

  /*close data file*/
  fclose(in);
  /*generate image information*/
  createImage(tilelist.c_str());
}


/*********************************************************************
World follow()
  adjusts the viewable area of the world to follow the specified
  object (attempts to keep them centered)
*********************************************************************/
void World::follow(Object &object)
{
  SDL_Rect dest;
  int wx, wy;
  int diffx, diffy;
  int maxx, maxy;

  //printf("FOLLOWING: %s\n", object.name.c_str());

  /*find where on screen the object WOULD be drawn*/
  /*translate from tile cell to world pixel coordinates*/
  wx = object.x * TILEWIDTH;
  wy = object.y * TILEHEIGHT;

  diffx = viewport.x - viewable.x;
  diffy = viewport.y - viewable.y;

  dest.x = wx + diffx;
  dest.y = wy + diffy;

  /*adjust by movement step offset, if we have steps left*/
  if(object.step)
  {
    switch(object.direction)
    {
      case UP:
        dest.y += ObjectStepSize[object.getSpeed()] * object.step;
        break;
      case DOWN:
        dest.y -= ObjectStepSize[object.getSpeed()] * object.step;
        break;
      case LEFT:
        dest.x += ObjectStepSize[object.getSpeed()] * object.step;
        break;
      case RIGHT:
        dest.x -= ObjectStepSize[object.getSpeed()] * object.step;
        break;
    }
  }

  /*calculate offset to center the object at 224x,256y*/
  /*handle x axis first*/
  maxx = w * TILEWIDTH - viewable.w;
  if(dest.x > 224)
  {
    /*scroll right*/
    viewable.x += dest.x-224;
    if(viewable.x > maxx) viewable.x = maxx;
  }
  else if(dest.x < 224)
    {
      /*scroll left*/
      viewable.x -= 224 - dest.x;
      if(viewable.x < 0) viewable.x = 0;
    }

  maxy = h * TILEHEIGHT - viewable.h;
  if(dest.y < 256)
  {
    /*scroll up*/
    viewable.y -= 256 - dest.y;
    if(viewable.y < 0) viewable.y = 0;
  }
  else if(dest.y > 256)
    {
      /*scroll down*/
      viewable.y += dest.y - 255;
      if(viewable.y > maxy) viewable.y = maxy;
    }
}

/*EOF*/

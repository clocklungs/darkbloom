#ifndef WORLD_HH
#define WORLD_HH

#include "object.hh"
#include <SDL2/SDL.h>
#include <string>

#define WWDISPLAY 15
#define WHDISPLAY 15
#define TILEHEIGHT 32
#define TILEWIDTH 32
#define WORLDHEIGHTMAX 100
#define WORLDWIDTHMAX 100
#define WORLDEMPTY 1 /*number for empty world tiles*/

class World {
  private:
    SDL_Surface *image; // world image data

    void createImage(const char *filename);

  public:
    unsigned char world[WORLDHEIGHTMAX][WORLDWIDTHMAX]; // world tile data

    SDL_Rect viewable; // viewable portion of the world
    SDL_Rect viewport; // screen position to display at
    int w;             // width in tiles
    int h;             // height in tiles
    std::string name;  // current world name

    World();                         // constructor
    ~World();                        // destructor
    void draw();                     // draw the world
    void load(const char *filename); // load map from file
    void follow(Object &object);     // adjust viewable area of world to accomodate object
};

extern World world;

#endif

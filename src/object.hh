#ifndef OBJECT_HH
#define OBJECT_HH

#include <string>
#include <stdio.h>
#include <SDL/SDL.h>
#include "datatypes.hh"
#include "path.hh"


typedef enum Collision {NONE=0, FULL=1, MOVE=2} Collision;
typedef enum Speed {SLOWED=0, WALK=1, RUN=2, HASTED=3} Speed;

const int ObjectSteps[4] = {16, 8, 4, 2};
const int ObjectStepSize[4] = {2, 4, 8, 16};


class Object
{
  protected:
    Speed speed;            //speed that the object is moving at
    Speed normal_speed;     //speed the object wants to move at
  public:
    std::string name;       //the object's name
    int x, y;               //x,y world grid/cell location
    bool visible;           //whether or not it is visible
    bool persistent;        //keep THIS version of the object,
    bool active;            //whether this object is in the current world
    float zindex;           //layer number
    int offset_x;           //offset from expected x drawing position
    int offset_y;           //offset from expected y drawing position
    SDL_Surface *image;     //image data, if any
    SDL_Rect frame_size;    //image dimensions (bounding box)
    std::string a_script;   //filename of script to run on activation
    std::string c_script;   //filename of script to run on collision
    Collision collidable;   //collision type for object

    int step;               //steps remaining in the transfer animation
    Direction direction;    //which direction the transfer is moving
    Stack move_stack;       //contains the directions from the pathfinding
    Path path;              //list of path waypoints

    Object();
    ~Object();
    bool load(const char *filename);
    void draw();
    Object* up(bool move=true);
    Object* down(bool move=true);
    Object* left(bool move=true);
    Object* right(bool move=true);
    Object* activate();
    void move();
    void setSpeed(int s);
    Speed getSpeed();
};


class AnimatedObject : public Object
{
  public:
                            //direction is used to determine animation to draw
    int frame;              //current frame of animation
    SDL_Surface *image[4];  //animation images for each direction
    int move_frame_start[4];//which frame the movement animation starts on
    int last_frame[4];      //max frame number
    bool moving;            //whether or not the object is moving to a new tile
    int elapsed;            //msecs since last frame

    AnimatedObject();
    ~AnimatedObject();
    bool load(const char *filename);
    void draw();
    void update();
};


/*node used by ObjectList/ObjectLayerList*/
class ObjectListNode
{
  public:
    float layer;            //object's zindex+y/100.0 (this is the sorting key)
    Object *object;         //pointer to an Object or AnimatedObject
    bool animated;          //whether it is an AnimatedObject
    std::string area;       //the area where this object is active
    ObjectListNode *next;   //next node in the list

    ObjectListNode()
    {
      object = NULL;
      animated = false;
      next = NULL;
    };
};


/********************************************************************
TODO:
  sort by area, then by the layer
  maybe add a pointer to the current area starting node, to make
  finding objects from/in the current area faster.
  maybe just make another linked list of areas, which contains the
  objectlists for each area
********************************************************************/
class ObjectList
{
  public:
    ObjectListNode *head;   //this is public so the objectlayerlist can add an
                            //entire objectlist easily. don't play w/ it.

    ObjectList() {head = NULL;};
    ~ObjectList();
    Object* add(const char *filename, int y);
    void draw();
    Object* collide(int x, int y, bool &move, Direction dir);
    Object* find(int x, int y);
    void activate();
    void flush();
    void clear();
    void debug();
};


class ObjectLayerList
{
  private:
    ObjectListNode *head;

  public:
    ObjectLayerList() {head = NULL;};
    ~ObjectLayerList();
    void add(Object &object);
    void add(AnimatedObject &object);
    void add(ObjectList &list);
    void draw();
};


extern ObjectList objects;

#endif

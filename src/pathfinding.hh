/*******************************************
*sweater puppy games w/ ED
*Darkbloom 574|2
*ai.hh
*ai header file
*used for pathfinding for NPC (and plyr?)
*This used the A* algorithim to find the
*shortest path through the grid.
*to use this just call
*path_find(SDL_Rect object, int findx, int findy)
*the object is the starting point with findx and findy
*being the end point respectivaly
*if it is unable to get there it will find the closet
*position it can get to
*data is pushed on to a stack. Stack is cleared
*when funcxtion is called.
********************************************/
#ifndef PATHFINDING_HH
#define PATHFINDING_HH

#include "object.hh"

#define MOVER 10 /*wieght to assign to squares*/
#define INFINITE 99999 /*infinte wieght value*/

void pathfind_rec(int x, int y, int findx, int findy);
void path_find(Object &object, int findx, int findy);
void closest_path(Object &object, int findx, int findy);

typedef struct blocks
  {
  /****************************************
  *blocks
  *all the information that needs to be held about
  *each block to determine its weight.
  *****************************************/
  int closed;/*-1 NA 0 open_slot 1 closed 2+ something in the way*/
  char dir;
  int g;
  int h;
  int f;
  } blocks;
  
typedef struct finder
  {
  /**************************************
  *finder
  *finder is used to hold avaible places
  *to search for a route
  ***************************************/
  int closed;
  int x;
  int y;
  int f;
  int h;
  } finder;

#endif

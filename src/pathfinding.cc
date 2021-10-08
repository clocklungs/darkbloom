/*******************************************
 *sweater puppy games w/ ED
 *Darkbloom 574|2
 *ai.cc
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
#include "pathfinding.hh"
#include "datatypes.hh"
#include "world.hh"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

blocks path[WORLDHEIGHTMAX][WORLDWIDTHMAX];       /*holds world data*/
finder open_slot[WORLDHEIGHTMAX * WORLDWIDTHMAX]; /*list of open_slot spots to go*/
finder tmp;                                       /*holder for where to go*/
int len;                                          /*current length of the open_slot array*/
int tmp_best;                                     /*position of tmp to be closed in open_slot*/

void pathfind_rec(int x, int y, int findx, int findy) {
    /********************************************************
     *A* algorithum very fast
     *used in a lot of games
     *determines the weight of a square
     *chceks all the lesser weights and sets direction
     *at the end you will have a path from the end location
     *to the start location. It is a shortest path algorithum
     *takes starting location and end location.
     *displays route from end location to start.
     *********************************************************/
    int done = 0;
    len = 0;

    /*starting node*/
    open_slot[len].closed = 1;
    open_slot[len].h = 10 * (abs((x)-findx) + abs(y - findy));
    open_slot[len].x = x;
    open_slot[len].y = y;

    len++;

    while (!done) { /*assumes that there is a path*/
        if (0 < x && path[y][x - 1].closed == -1) {
            if (x - 1 == findx && y == findy) { /*done*/
                done = 1;
            }
            path[y][x - 1].closed = 0;
            path[y][x - 1].g = path[y][x].g + MOVER;
            path[y][x - 1].h = 10 * (abs((x - 1) - findx) + abs(y - findy));
            path[y][x - 1].f = path[y][x - 1].g + path[y][x - 1].h;
            path[y][x - 1].dir = 'r';
            open_slot[len].closed = 0;
            open_slot[len].x = x - 1;
            open_slot[len].y = y;
            open_slot[len].f = path[y][x - 1].f;
            open_slot[len].h = path[y][x - 1].h;
            len++;
        }

        if (x < world.w && path[y][x + 1].closed == -1) {
            if (x + 1 == findx && y == findy) { /*done*/
                done = 1;
            }
            path[y][x + 1].closed = 0;
            path[y][x + 1].g = path[y][x].g + MOVER;
            path[y][x + 1].h = 10 * (abs((x + 1) - findx) + abs(y - findy));
            path[y][x + 1].f = path[y][x + 1].g + path[y][x + 1].h;
            path[y][x + 1].dir = 'l';
            open_slot[len].closed = 0;
            open_slot[len].x = x + 1;
            open_slot[len].y = y;
            open_slot[len].f = path[y][x + 1].f;
            open_slot[len].h = path[y][x + 1].h;
            len++;
        }

        if (0 < y && path[y - 1][x].closed == -1) {
            if (x == findx && y - 1 == findy) { /*done*/
                done = 1;
            }
            path[y - 1][x].closed = 0;
            path[y - 1][x].g = path[y][x].g + MOVER;
            path[y - 1][x].h = 10 * (abs(x - findx) + abs(y - findy));
            path[y - 1][x].f = path[y - 1][x].g + path[y - 1][x].h;
            path[y - 1][x].dir = 'd';
            open_slot[len].closed = 0;
            open_slot[len].x = x;
            open_slot[len].y = y - 1;
            open_slot[len].f = path[y - 1][x].f;
            open_slot[len].h = path[y - 1][x].h;
            len++;
        }

        if (y < world.h && path[y + 1][x].closed == -1) {
            if (x == findx && y + 1 == findy) { /*done*/
                done = 1;
            }
            path[y + 1][x].closed = 0;
            path[y + 1][x].g = path[y][x].g + MOVER;
            path[y + 1][x].h = 10 * (abs(x - findx) + abs(y - findy));
            path[y + 1][x].f = path[y + 1][x].g + path[y + 1][x].h;
            path[y + 1][x].dir = 'u';
            open_slot[len].closed = 0;
            open_slot[len].x = x;
            open_slot[len].y = y + 1;
            open_slot[len].f = path[y + 1][x].f;
            open_slot[len].h = path[y + 1][x].h;
            len++;
        }

        path[y][x].closed = 1; /*puts current position on closed list*/

        tmp.closed = -1;
        tmp.x = -1;
        tmp.y = -1;
        tmp.f = INFINITE; /*infinity*/
                          //    empty(); /*empty stack before using*/
        for (int i = 0; i < len; i++) {
            if (open_slot[i].closed == 0) {
                if (open_slot[i].f < tmp.f) {
                    tmp.x = open_slot[i].x;
                    tmp.y = open_slot[i].y;
                    tmp.f = open_slot[i].f;
                    tmp_best = i;
                }
            }
        }
        open_slot[tmp_best].closed = 1;
        x = tmp.x;
        y = tmp.y;

        if ((x == findx && y == findy) || tmp.f == INFINITE) { /*done*/
            done = 1;
        }
    } /*end while*/
} /*void pathfind_rec(int x, int y, int findx, int findy)*/

void path_find(Object &object, int findx, int findy) {
    /*******************************************************
     *path_find
     *initialised varibles to be used to find the route
     *******************************************************/
    int h, w;
    int done = 0;
    int tmpx = findx;
    int tmpy = findy;
    ObjectListNode *search;

    /*  printf("DEBUG: start path_find\n");*/
    for (h = 0; h < WORLDHEIGHTMAX; h++) { /*inits world*/
        for (w = 0; w < WORLDWIDTHMAX; w++) {
            if (world.world[h][w] % 2 == 0) { /*block avaibale for moving into*/
                path[h][w].closed = -1;
                path[h][w].g = 0;
                path[h][w].h = -1;
                path[h][w].f = -1;
                path[h][w].dir = '\0';
            } else {
                path[h][w].closed = 2;
                /*can not go into block*/
            }
        }
    }
    /*
    printf("objx %d objy %d arrayx %d arrayy %d\n",
              object.x,object.y, findx, findy);

    printf("DEBUG: funct\n");*/
    /*read in objects in world when implemented here*/
    search = objects.head;
    while (search != NULL) {
        /*check if the object is in the current map, active, and collidable*/
        if ((search->area == world.name) && (search->object->active) && (search->object->collidable != NONE)) {
            /*set the path to something >= 2*/
            path[search->object->y][search->object->x].closed = 3;
        }
        search = search->next;
    }

    /*calls the function to find shortest path*/
    pathfind_rec(object.x, object.y, findx, findy);

    /*  printf("DEBUG: start outloop\n");*/
    if (path[tmpy][tmpx].dir == '\0') {
        //    printf("no path\n");
        closest_path(object, findx, findy);
    } else {
        while (!done) { /*prints out shortest path*/
            if (path[tmpy][tmpx].dir == 'u') {
                tmpy--;
                object.move_stack.push(DOWN);
            } else if (path[tmpy][tmpx].dir == 'd') {
                tmpy++;
                object.move_stack.push(UP);
            } else if (path[tmpy][tmpx].dir == 'l') {
                tmpx--;
                object.move_stack.push(RIGHT);
            } else if (path[tmpy][tmpx].dir == 'r') {
                tmpx++;
                object.move_stack.push(LEFT);
            }
            if (tmpx == object.x && tmpy == object.y) { /*checks if it found the end point*/
                done = 1;
            }
        } /*while(tmpx==object.x && object.y == tmpy)*/
    }     /*end if(path[tmpy][tmpx].dir=='\0')*/

} /*end void path_find(SDL_Rect object, int findx, int findy) */

void closest_path(Object &object, int findx, int findy) {
    int done = 0;
    int tmpy;
    int tmpx;

    tmp.h = INFINITE;

    for (int i = 0; i < len; i++) {   /*checks for lowest wieght value*/
        if (open_slot[i].h < tmp.h) { /*found new lowest wieght*/
            tmp.h = open_slot[i].h;
            tmp.x = open_slot[i].x;
            tmp.y = open_slot[i].y;
        }
    }
    tmpx = tmp.x;
    tmpy = tmp.y;

    /*printf("DEBUG: prints out from closet position\n"); */

    while (!done) { /*prints out shortest path*/
        if (path[tmpy][tmpx].dir == 'u') {
            tmpy--;
            object.move_stack.push(DOWN);
        } else if (path[tmpy][tmpx].dir == 'd') {
            tmpy++;
            object.move_stack.push(UP);
        } else if (path[tmpy][tmpx].dir == 'l') {
            tmpx--;
            object.move_stack.push(RIGHT);
        } else if (path[tmpy][tmpx].dir == 'r') {
            tmpx++;
            object.move_stack.push(LEFT);
        }
        if (tmpx == object.x && tmpy == object.y) { /*checks if it found the end point*/
            done = 1;
        }
    } /*end while*/
} /* void closest_path(int x, int y, int num) */

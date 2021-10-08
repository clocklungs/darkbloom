#ifndef PATH_HH
#define PATH_HH

#include <SDL/SDL.h>


class PathNode
{
  public:
    Uint8 x;
    Uint8 y;
    PathNode *next;

    PathNode()
    {
      x = 0;
      y = 0;
      next = NULL;
    };
};


class Path
{
  private:
    PathNode *head;
    PathNode *curr;
    int loop;
  public:
    Path()
    {
      head = NULL;
      curr = NULL;
      loop = 0;
    };
    ~Path();
    bool load(const char *filename);
    void add(int x, int y, bool relative=false);
    PathNode* next();
    void clear();
};

#endif

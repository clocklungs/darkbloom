#include "path.hh"


/********************************************************************
Path::~Path()
  we have a clear function, lets use it.
********************************************************************/
Path::~Path()
{
  clear();
}


/********************************************************************
PathNode* Path::next()
  returns a pointer to the next PathNode/waypoint in the path, if any
  else returns NULL
********************************************************************/
PathNode* Path::next()
{
  PathNode *temp;

  /*check if there is any path*/
  if(head == NULL) return NULL;

  /*if we've exausted the path, clear it (we don't need it anymore)
  we don't get here unless we've run out of loops*/
  if(curr == NULL)
  {
    clear();
    return NULL;
  }

  /*we're still working on a path. save next PathNode*/
  temp = curr;

  /*prepare the curr pointer for the next iteration*/
  curr = curr->next;
  if(curr == NULL && loop--) curr = head;

  return temp;
}


/********************************************************************
void Path::clear()
  just your average list traversal deleting the nodes as we go
********************************************************************/
void Path::clear()
{
  while(head != NULL)
  {
    curr = head;
    head = head->next;
    delete curr;
  }
  head = NULL;
  curr = NULL;
  loop = 0;
}


/*EOF*/

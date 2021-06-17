#include <stdlib.h>
#include <stdio.h>
#include "datatypes.hh"


Stack::Stack()
  {
  stack_len=0;
  }


void Stack::empty()
/*empties stack*/
  {
  stack_len=0;
  }

bool Stack::is_empty()
  {
  if(stack_len==0)
    {
    return false;
    }
  else
    {
    return true;
    }
  }

bool Stack::is_full()
  {
  if(stack_len==STACKSIZE)
    {
    return false;
    }
  else
    {
    return true;
    }
  }

bool Stack::push(Direction added)
  {
  if(stack_len==STACKSIZE)
    {
    return false;
    }
  else
    {
    stack_len++;
    stack[stack_len]=added;
    return true;
    }
  }

Direction Stack::pop()
  {
  if(stack_len==0)
    {
    fprintf(stderr, "STACK UNDERFLOW\n");
    return DOWN;
    }
  else
    {
    stack_len--;
    return stack[stack_len+1];
    }
  }

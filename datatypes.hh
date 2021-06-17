#ifndef DATATYPES_HH
#define DATATYPES_HH

#define STACKSIZE 100

typedef enum Direction {DOWN=0, UP=1, LEFT=2, RIGHT=3};

class Stack
{
  public:
    Direction stack[STACKSIZE];
    int stack_len;

    Stack();
    void empty();
    bool is_empty();
    bool is_full();
    bool push(Direction added);
    Direction pop();
};

#endif

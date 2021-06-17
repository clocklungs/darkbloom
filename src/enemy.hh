#ifndef ENEMY_HH
#define ENEMY_HH

#define MAX_ENEMIES 10

#include <string>


class Enemy
{
  public:
    std::string name;           //name (only first 10 chars displayed, usually)
    int hp;                     //life (maximum)
    int strength;               //how hard it hits
    int agility;                //dodging and attacking ability
    int xp;                     //how many experience points it is worth
    std::string graphic_idle;   //filename of idle graphic
    std::string graphic_attack; //filename of attack graphic
    int offset_x;               //how much to offset graphic display (x axis)
    int offset_y;               //how much to offset graphic display (y axis)

    bool load(const char *filename);  //loads enemy definition from a file
};


int load_enemies(const char *filename);
bool load_boss(const char *filename);


extern Enemy enemies[MAX_ENEMIES];  //array of area enemies
extern Enemy boss;                  //current special (boss) enemy
extern int enemy_count;             //how many area enemies are loaded

#endif

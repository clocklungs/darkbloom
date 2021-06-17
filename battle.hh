/*******************************************
*sweater puppy games w/ ED
*Darkbloom 574|2
*battle.hh
*battle header file
*This displays the battles that you get into
*To you know fight in
*
********************************************/
#ifndef BATTLE_HH
#define BATTLE_HH

#define MAXPLYRS 3 /*max amounts of plyr chars on battlefield*/
#define MAXNMYS 4 /*max amounts of nmy chars on battlefield*/
#define PLYRX 340 /*starting x amount plyrs x*/
#define PLYRXMOVE 25 /*how far to space plyrs on battlefiled x.ly*/
#define NMYXMOVE 25 /*how far to space nmy on battlefiled x.ly*/
#define PLYRY 20  /*starting x amount plyrs y*/
#define PLYRIMGWIDTH 85 /*players width*/
#define PLYRIMGHEIGHT 155 /*players hieght*/
#define NMYIMGWIDTH 85  /*nmys width*/
#define NMYIMGHEIGHT 155  /*nmys height*/
#define CURSORW 32  /*battle cursors width*/
#define CURSORH 32  /*battle cusrsors height*/
#define BMENU1X 300 /*battle menu1s x location*/
#define BMENU1Y 325 /*battle menu1s y location*/
#define BMENUCHOICES 2
#define BMENUOFFSET 20
#define ATTACKLOOP 20
#define MAGICLOOP 20
#define CHARIMAGES 3
#define NMYIMAGES 2
#define DMGNUMLEN 10
#define ATTACKDEL 90
#define MAGICDEL 50
#define MAGICS 3
#define MAGICFRAMES 4
#define MAGICX -85
#define MAGICY 0
#define MAGICH 155
#define MAGICW 85


void battlefield_init(int nmy, int boss);/*inits the things needed in battle*/
void battle_destruct();/*frees up the images used in battle*/
bool battleloop(int nmy,int boss);
int battleing();
void attack_cursor_l(); /*moves cursors to differnet enemies*/
void attack_cursor_r(); /*moves cursors to differnet enemies - other direction*/
void battlemenu(int menu_level); /*displays option of the battle menu*/
int bmenuselect(int selected,int menu_level); /*displays current menu choice*/
void playerturn(int selected);
int pattacking(int player,int enemy,int selected_menu_choice, int menu_level);
void eattacking(int player,int enemy, int selected_menu_choice);
bool blitscreen(int select_enemy,int selected_menu_choice);


struct bworld
  {
  SDL_Surface *image, *bmenu0, *bmenu1;
  SDL_Rect rect, bmenu_rect;
  };

struct bcusor
  {
  SDL_Surface *image;
  SDL_Rect rect;
  int selected;
  };
  
struct bplayers
  {
  SDL_Surface *image[CHARIMAGES];
  SDL_Rect wrect, plyr_rect;
  int delay, delaymax;
  int attacking;
  };

struct spellsani
  {/*spell animations*/
  SDL_Surface *magic;
  SDL_Rect wrect, spell;
  };

struct bnmys
  {
  SDL_Surface *image[NMYIMAGES];
  SDL_Rect wrect, nmy_rect;
  int attacking;
  int hp,
      strength,
      agility,
      xp,
      offset_x,
      offset_y,
      dead;
  };

int hitfor(int a);/*temp - ed has other stuff*/


#endif

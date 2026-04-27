#include <curses.h>
#include "sprites.h"

// the most crucial window here
extern WINDOW* game;

// engine functions
extern WINDOW* create_win(int h, int w, int y, int x, bool border);
extern WINDOW* genspr(SPRITE chr);
extern void movespr(SPRITE spr, int y, int x);
extern int shoot(SPRITE src, SPRITE dst);
extern void kill(SPRITE spr);
extern void health(SPRITE spr);

extern void newlvl(void);
extern void enemctrl(void);

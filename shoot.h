#include <curses.h>
#include "sprites.h"
#define LVL_MAX 12
// ^ maximum level of the game,
// feel free to crank this up

// the most crucial window here
extern WINDOW* game;

// generic input buffer for wgetch()
extern int key;

// general engine functions
extern WINDOW* create_win(int h, int w, int y, int x, bool border);
extern WINDOW* genspr(SPRITE chr);
extern void movespr(SPRITE spr, int y, int x);
extern int  shoot(SPRITE src, SPRITE dst);
extern void kill(SPRITE spr);
extern void health(SPRITE spr);
extern void transition(void);

// still part of it, but a little more specific
extern void newlvl(int level);
extern void titlescr(void);
extern void gameover(int level);
extern void enemctrl(void);
extern void endgame(int level);

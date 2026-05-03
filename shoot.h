#include <curses.h>
#include "sprites.h"
#define LVL_MAX 12
// ^ maximum level of the game,
// feel free to crank this up

// the most crucial window here
extern WINDOW* game;

// transition types, these are
// all we have for now
typedef enum trans_t {
	T_CURTAIN,
	T_DEBRIS
} trans_t;

// generic input buffer for wgetch()
extern int key;

// starting level
extern int  level;
extern int* lvlptr;
// ^ and its pointer

// general engine functions
extern WINDOW* create_win(int h, int w, int y, int x, bool border);
extern WINDOW* newspr(SPRITE chr);
extern void mvspr(SPRITE spr, int y, int x);
extern int  shoot(SPRITE src, SPRITE dst);
extern void kill(SPRITE spr);
extern void health(SPRITE spr);
extern void transition(trans_t transition);

// still part of it, but a little more specific
extern void newlvl(void);
extern void counter(void);
extern void titlescr(void);
extern void gameover(void);
extern void enemctrl(void);
extern void endgame(void);

#ifndef SHOOT_H
#define SHOOT_H
#include <curses.h>
#include "sprites.h"
#define LVL_MAX 12
// ^ maximum level of the game,
// feel free to crank this up

// the most crucial window here
extern WINDOW* game;

// transition types, this is
// all we have for now
typedef enum trans_t {
	T_CURTAIN,
	T_DOTSYNC,
	T_DEBRIS
} trans_t;

// generic input buffer for wgetch()
extern int key;

// level tracking
extern unsigned char levels;
extern uintptr_t level;

// general engine functions
extern WINDOW* create_win(int h, int w, int y, int x, bool border);
extern WINDOW* newspr(SPRITE spr);
extern void mvspr(SPRITE spr);
extern unsigned char shoot(SPRITE src, SPRITE dst);
extern void kill(SPRITE spr);
extern void health(SPRITE spr);
extern void transition(trans_t transition);

// still part of it, but a little more specific
extern void enemctrl(void);
extern void newlvl(void);
extern void counter(void);
extern void titlescr(void);
extern void gameover(void);
extern void ending(void);
extern void endgame(bool aborted);
#endif // SHOOT_H

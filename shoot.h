#include <curses.h>
#include "sprites.h"

typedef struct HUD {
	WINDOW* top;
	WINDOW* bottom;
} HUD;

// the most crucial windows here
extern WINDOW* game;
extern HUD hud;

// engine functions
extern WINDOW* create_win(int h, int w, int y, int x, bool border);
extern WINDOW* genspr(SPRITE chr);
extern void movespr(SPRITE spr, int y, int x);
extern int shoot(SPRITE player, SPRITE enemy);
extern void boom(SPRITE spr);
extern void kill(SPRITE spr);
extern void hit(SPRITE spr);
extern void health(SPRITE spr, WINDOW* dsp);

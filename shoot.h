#include <curses.h>
#include "sprites.h"

// the most crucial windows here
extern WINDOW* game;
extern WINDOW* hud;

// engine functions
extern WINDOW* create_win(int h, int w, int y, int x, bool border);
extern WINDOW* genspr(SPRITE chr);
extern void movespr(SPRITE spr, int y, int x);
extern bool shoot(SPRITE player, SPRITE enemy);
extern void boom(SPRITE chr);
extern void collide(SPRITE spr);

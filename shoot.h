#include <curses.h>
#include "sprites.h"

// the most crucial windows here
extern WINDOW* game;
extern WINDOW* hud;

// engine functions
extern WINDOW* create_win(int h, int w, int y, int x, bool border);
extern WINDOW* genspr(SPRITE chr);
extern void movespr(WINDOW* spr, const char* skin, int y, int x);
extern bool shoot(WINDOW* tgt, SPRITE player, SPRITE enemy);
extern void boom(WINDOW* spr, SPRITE chr);
extern void collide(WINDOW* tgt);

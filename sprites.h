#ifndef SPRITES_H
#define SPRITES_H
#include <curses.h>

// a sprite here has a
typedef struct SPRITE {
	unsigned short y, x; // starting and current position
	const unsigned short h, w; // height & width
	unsigned char hit[2]; // hitbox (0: y, 1: x)
	unsigned char hp; // health
	const char* skin; // how it looks like
	WINDOW* win; // and where it's from
	WINDOW* hud; // and its own HUD
} SPRITE;

// and this is what we have for now
extern SPRITE player;
extern SPRITE enemy;
extern SPRITE bullet;

// one of these must be selected
// at random, currently only the
// enemy sprite uses this
typedef enum MOVE {
	MV_DOWN,
	MV_LEFT,
	MV_RIGHT,
	MV_SHOOT,
	MV_UP
} move_t;
#endif // SPRITES_H

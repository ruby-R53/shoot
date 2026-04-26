#include <curses.h>

// a sprite here has a
typedef struct SPRITE {
	int y, x; // starting position
	unsigned int h, w; // height, width
	unsigned int hp; // health
	const char* skin; // and how it looks like
	WINDOW* win; // and where it's from
	WINDOW* hud; // and its own HUD
} SPRITE;

// and this is what we have for now
extern SPRITE player;
extern SPRITE enemy;
extern SPRITE bullet;

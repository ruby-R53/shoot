#include <curses.h>

// a sprite here has a
typedef struct SPRITE {
	int y, x; // starting position
	int h, w; // height, width
	int hp; // health
	const char* skin; // and how it looks like
	WINDOW* win; // and where it's from
} SPRITE;

// and this is what we have for now
extern SPRITE player;
extern SPRITE enemy;
extern SPRITE bullet;

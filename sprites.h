// a sprite here has a
typedef struct SPRITE {
	int y, x; // starting position
	int h, w; // height, width
	const char* skin; // and how it looks like
} SPRITE;

// and this is what we have for now
extern SPRITE player;
extern SPRITE enemy;
extern SPRITE bullet;

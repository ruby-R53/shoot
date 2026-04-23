#include "sprites.h"

// these parameters are all passed to `genspr()`, however
// FIXME should there be a way to calculate the sprites'
// sizes rather than hand-typing them?

SPRITE bullet = {
	.skin = "*",
	.h = 1,
	.w = 1
};

SPRITE player = {
	.skin = "-=^=-",
	.h = 1,
	.w = 5,
	.y = 45,
	.x = 75/2
};

SPRITE enemy = {
	.skin = "-=v=-",
	.h = 1,
	.w = 5,
	.y = 5,
	.x = 75/2
};

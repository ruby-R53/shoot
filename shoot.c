#include <unistd.h>
#include <stdlib.h>
#include "shoot.h"

WINDOW* game;
WINDOW* hud;

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

WINDOW* create_win(int h, int w, int y, int x, bool border) {
	WINDOW* win;
	win = newwin(h, w, y, x);
	if (border) box(win, 0, 0);
	wrefresh(win);

	return win;
}

WINDOW* genspr(SPRITE chr) {
	WINDOW* spr;
	spr = derwin(game, chr.h, chr.w, chr.y, chr.x);
	wprintw(spr, "%s", chr.skin);
	wrefresh(spr);

	return spr;
}

void movespr(WINDOW* spr, const char* skin, int y, int x) {
	werase(spr);
	touchwin(game);
	mvderwin(spr, y, x);

	wprintw(spr, "%s", skin);
	wnoutrefresh(spr);
	doupdate();
}

bool shoot(WINDOW* tgt, SPRITE player, SPRITE enemy) {
	bullet.y = player.y - 1;
	bullet.x = player.x + 2;
	WINDOW* bullet_w = genspr(bullet);

	for (; bullet.y >= 1; --bullet.y) {
		wmove(game, bullet.y, bullet.x);
		movespr(bullet_w, bullet.skin, bullet.y, bullet.x);
		wrefresh(game);

		if (bullet.y == enemy.y &&
			bullet.x == (enemy.x+2)) {
			if (collide(tgt)) tgt = NULL;
			goto cleanup;
		}

		usleep(5000);
	}

cleanup:
	werase(bullet_w);
	wrefresh(bullet_w);
	delwin(bullet_w);

	return collide(tgt);
}

void boom(WINDOW* spr, SPRITE chr) {
	int y = 0;
	int x = 0;
	int tick = 1000;

	while (tick >= 0) {
		y = rand() % 50;
		x = rand() % 80;
		mvwaddch(game, y, x, '.');
		wrefresh(game);
		usleep(500);
		--tick;
	}

	wclear(game);
	box(game, 0, 0);
	movespr(spr, chr.skin, chr.y, chr.x);
}

bool collide(WINDOW* tgt) {
	if (tgt != NULL) {
		werase(tgt);
		wrefresh(tgt);
		delwin(tgt);
		return true;
	} else
		return false;
}

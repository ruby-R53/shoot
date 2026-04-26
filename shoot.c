#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "shoot.h"

// still nothing for those, for now
WINDOW* game;

// create a window with
// h height,
// w width,
// at
// y location,
// and x location,
// with or without border!
WINDOW* create_win(int h, int w, int y, int x, bool border) {
	WINDOW* win;
	win = newwin(h, w, y, x);
	if (border) box(win, 0, 0);
	wrefresh(win);

	return win;
}

// generate sprite from character data
WINDOW* genspr(SPRITE chr) {
	WINDOW* spr;
	// make them windows so that ncurses understands
	// them
	spr = derwin(game, chr.h, chr.w, chr.y, chr.x);
	wprintw(spr, "%s", chr.skin);
	wrefresh(spr);

	return spr;
}

// move it
void movespr(SPRITE spr, int y, int x) {
	werase(spr.win); // first, delete its trail
	touchwin(game); // but the parent window must be made aware of that
	mvderwin(spr.win, y, x); // before its derived one (the sprite) can actually move

	// now actually show the thing at its specific
	// coordinates
	wprintw(spr.win, "%s", spr.skin);
	wnoutrefresh(spr.win);
	doupdate();
}

// what the game is about, we need to specify
// where we're shooting from and where the
// opponent is
int shoot(SPRITE src, SPRITE dst) {
	// use src's coordinates as the base
	bullet.y = src.y;
	bullet.x = src.x + 2;
	// ^ FIXME there needs to be a better way to say it's
	// the sprite's center that we want

	// spawn the bullet already
	bullet.win = genspr(bullet);

	// check if the enemy is shooting to flip the
	// bullet's direction, otherwise don't do anything
	bool flip = (src.win == player.win) ? false : true;

	// move that bullet thing or down, depending on
	// who's shooting!
	for (; (flip ? bullet.y <= 48 : bullet.y >= 1); (flip ? ++bullet.y : --bullet.y)) {
		wmove(game, bullet.y, bullet.x);
		movespr(bullet, bullet.y, bullet.x);
		wrefresh(game);

		// if our target (dst) is around, check if the bullet
		// hit its hitbox (its y position and horizontal center)
		if (dst.hp != 0 &&
			bullet.y == dst.y &&
			bullet.x == (dst.x+2)) {
			--dst.hp;
			health(dst);
			if (dst.hp == 0) kill(dst);
			goto cleanup;
			// it makes sense that it disappears after hitting
			// something before the wall tho'
		}

		usleep(5000);
	}

cleanup:
	movespr(src, src.y, src.x);
	werase(bullet.win); // make it disappear!
	wrefresh(bullet.win);
	delwin(bullet.win); // let curses know it disappeared!
	if (dst.hp > 0) movespr(dst, dst.y, dst.x);
	// ^ and redraw the thing in case it got hit but is still alive

	return dst.hp; // and that will be kept track of in `main()`
}

// if someone got killed, make ncurses actually kill them too
void kill(SPRITE spr) {
	werase(spr.win); // erase the target
	wrefresh(spr.win);
	delwin(spr.win); // make curses now the target is dead
}

// health status for each sprite
void health(SPRITE spr) {
	if (spr.hp == 0) { // show a message to the player
		mvwprintw(spr.hud, 0, 0, "Killed!");
		wrefresh(spr.hud);
		usleep(125000);
		werase(spr.hud);
	} else // pad it with zeros, i want it to be higher at a later point
		mvwprintw(spr.hud, 0, 0, "HP: %02d", spr.hp);

	wrefresh(spr.hud);
}

void enemctrl(void) {
	typedef enum MOVE {
		MV_SHOOT,
		MV_LEFT,
		MV_DOWN,
		MV_UP,
		MV_RIGHT
	} move_t;

	srand(time(0)); // make the moves "truly" random
	move_t move = rand() % 4;
	switch(move) {
		case MV_SHOOT:
			if (enemy.y < 48) player.hp = shoot(enemy, player);
			break;

		case MV_UP:
			if (enemy.y >= 2) movespr(enemy, --enemy.y, enemy.x);
			break;

		case MV_DOWN:
			if (enemy.y <= 47) movespr(enemy, ++enemy.y, enemy.x);
			break;

		case MV_LEFT:
			if (enemy.x >= 2) movespr(enemy, enemy.y, --enemy.x);
			break;

		case MV_RIGHT:
			if (enemy.x <= 73) movespr(enemy, enemy.y, ++enemy.x);
			break;
	}

	return;
}

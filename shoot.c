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
		mvwprintw(spr.hud, 0, ((getmaxx(spr.hud) - 7) / 2), "Killed!");
		wrefresh(spr.hud);
		usleep(125000); // for .125 seconds
		werase(spr.hud);
	} else // pad it with zeros, i want it to be higher at a later point
		mvwprintw(spr.hud, 0, ((getmaxx(spr.hud) - 6) / 2), "HP: %02d", spr.hp);

	wrefresh(spr.hud);
}

// play a cool little transition between
// stages and moments
// should more kinds of it be implemented?
void transition(void) {
	int y = 0;
	int x = 0;
	int tick = 1000;
	// 1k iterations seems reasonable, it doesn't
	// take very long to happen and it doesn't
	// fill the screen so much
	int backup[2][1000];
	unsigned int backpos = 0;
	// for smoothing the transition out, this will
	// store each of y and x's positions

	// initialize the RNG
	srand(time(NULL));

	while (tick >= 0) {
		// to choose a random spot to fill
		y = rand() % 49;
		x = rand() % 79;

		// and these are the boundaries
		if (y == 0) ++y;
		if (x == 0) ++x;

		// the screen filler is a dot, like
		// some kind of debris
		mvwaddch(game, y, x, '.');
		wrefresh(game);
		usleep(500);

		// for transferring to the array
		backpos = 1000 - tick;

		// store current positions for later
		backup[0][backpos] = y;
		backup[1][backpos] = x;

		--tick; // one dot printed, 999 more to go
	}

	// restore the ticker for the next step,
	// erasing each dot out
	tick = 1000;

	while (tick >= 0) {
		// and restore the array indexer
		backpos = 1000 - tick;

		// so that we can unfill the spots back
		mvwaddch(game, backup[0][backpos], backup[1][backpos], ' ');
		wrefresh(game);
		usleep(500);

		--tick; // one dot removed, 999 more to go
	}

	// and finally clear the remaining mess in case
	// any message got printed as well
	wclear(game);
	box(game, 0, 0); // which means redrawing the box

	return;
}

// the enemy's movements, driven by RNG
void enemctrl(void) {
	// one of these must be selected at random
	typedef enum MOVE {
		MV_SHOOT,
		MV_LEFT,
		MV_DOWN,
		MV_UP,
		MV_RIGHT
	} move_t;

	srand(time(NULL)); // make the moves "truly" random
	move_t move = rand() % 4;
	// ^ FIXME there actually seems to be some bias towards
	// moving to the same direction over and over
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

// what to do on a new level
void newlvl(int level) {
	transition(); // play a cool transition

	// regenerate the enemy sprites,
	// along with an upgraded HP
	enemy.win = genspr(enemy);
	enemy.hp  = level + 5;

	// display both health meters
	health(enemy);
	health(player);

	// because of the transition animation,
	// the screen got cleared, so redraw
	// the player too
	movespr(player, player.y, player.x);

	return;
}

// we lost, what's next?
bool gameover(int level) {
	int key = 0;
	transition(); // play that cool transition tho'

	// and here's the menu itself
	mvwprintw(game, ((50 - 3) / 2), ((80 - 15) / 2), "Mission failed!");
	mvwprintw(game, ((50 - 1) / 2), ((80 - 16) / 2), "Try again? [Y/N]");
	wrefresh(game);

	while (player.win == NULL) { // loop it
		key = wgetch(game);
		switch(key) {
			case 'y': // just start a new game like nothing had happened
				player.win = genspr(player);
				player.hp  = 4;
				newlvl(level);
				return false;
				break;

			case 'n': // or end it all
				transition();
				endgame(level);
				return true;
				break;
		}
	}

	return false; // just so that GCC doesn't complain
}

void endgame(int level) {
	// finish curses now that the player's done
	endwin();

	// and tell them where they stopped, really not sure
	// how useful or cool this is
	if (level > 0) printf("Quit at level %d!\n", level);
	else printf("You won, thank you for playing!\n");

	// and we're done
	exit(0);
}

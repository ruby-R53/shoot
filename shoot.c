#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "art.h"
#include "shoot.h"

// still nothing for those, for now
WINDOW* game;

// but initialize the input buffer
int key = 0;

// and the level
int level = 1;
int *lvlptr = &level;
// ^ with a pointer so that it can be
// globally updated

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
	wrefresh(win); // and show it

	return win;
}

// generate sprite from character data
WINDOW* genspr(SPRITE chr) {
	WINDOW* spr;
	// make it a derived windows so that ncurses understands it
	spr = derwin(game, chr.h, chr.w, chr.y, chr.x);
	wprintw(spr, "%s", chr.skin);
	wrefresh(spr); // and show it

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
	bullet.x = src.x + getmaxx(src.win) / 2;
	// ^ the sprite's horizontal center, this is where
	// bullets will always get shot out from

	// spawn the bullet already
	bullet.win = genspr(bullet);

	// check if the enemy is shooting to flip the
	// bullet's direction, otherwise don't do anything
	bool flip = (src.win == player.win) ? false : true;

	// move that bullet thing or down, depending on
	// who's shooting!
	for (; (flip ? bullet.y <= 48 : bullet.y >= 1); (flip ? ++bullet.y : --bullet.y)) {
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
void transition(trans_t transition) {
	int y = 0;
	int x = 0;

	switch(transition) {
		// wipe the screen with columnfuls of dots
		// then erase them column by column
		case T_CURTAIN:
			chtype fill[2] = { '.', ' ' };
			for (int i = 0; i <= 1; ++i) {
				for (x = 1; x <= 78; ++x) {
					for (y = 1; y <= 48; ++y)
						mvwaddch(game, y, x, fill[i]);

					wrefresh(game);
					// so that it immediately gets
					// to the next part
					if (x < 78) usleep(10000);
				}
				// also stop to show a "get ready"
				// kind of message, making sure the
				// background is still all dots
				if (i == 0) {
					mvwprintw(game, 50/2, (80-22)/2, "Battle level %02d START!", level);
					wrefresh(game);
					usleep(500000); // for .5 seconds
				}
			}
			break;

		// or fill the screen with dots
		case T_DEBRIS:
			int tick = 1000;
			// 1k iterations seems reasonable, it doesn't
			// take very long to happen and it doesn't
			// fill the screen so much
			int backup[2][1000];
			unsigned int backpos = 0;
			// for smoothing the transition out, these will
			// store each of y and x's positions

			// initialize the RNG
			srandom(time(NULL));

			while (tick >= 0) {
				// to choose a random spot to fill,
				// with the walls in mind
				y = 1 + random() % 48;
				x = 1 + random() % 78;

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
			break;
	}

	return;
}

// the enemy's movements, driven by RNG
void enemctrl(void) {
	// one of these must be selected
	// at random
	typedef enum MOVE {
		MV_SHOOT,
		MV_LEFT,
		MV_UP,
		MV_DOWN,
		MV_RIGHT
	} move_t;

	srandom(time(NULL)); // make the moves "truly" random
	move_t move = random() % 4;
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
void newlvl(void) {
	if (level <= LVL_MAX) {
		transition(T_CURTAIN); // play a cool transition

		// regenerate the enemy sprites,
		// along with an upgraded HP
		// and new positions
		srandom(time(NULL));
		enemy.y   = 2 + random() % 45;
		enemy.x   = 2 + random() % 73;
		enemy.hp  = level + 5;
		enemy.win = genspr(enemy);

		// and +1 the player's HP at
		// every 2 levels
		player.hp += level % 2;

		// display both health meters
		health(enemy);
		health(player);

		// because of the transition animation,
		// the screen got cleared, so redraw
		// the player too
		movespr(player, player.y, player.x);
	}

	// and finally, update the counter
	counter();
}

// level counter
void counter(void) {
	if (level <= LVL_MAX)
		mvwprintw(player.hud, 1, 0, "Level: %02d", level);
	else // you cleared the game!
		mvwprintw(player.hud, 1, 0, "All Clear!");

	wrefresh(player.hud);
}

// how the title screen is displayed
void titlescr(void) {
	// show the title art with a cool
	// little animation
	printart(&title, 0, 0);

	// and show the keys to the user,
	// in italics just to look cooler
	wattron(game, A_ITALIC);
	mvwprintw(game, 43, (80-10)/2, "Z - begin!");
	mvwprintw(game, 44, (80-10)/2, "Q -  quit!");
	wrefresh(game);
	wattroff(game, A_ITALIC);

	// then handle the keys
	key = wgetch(game);
	switch(key) {
		case 'z':
			transition(T_CURTAIN);
			return;
			break;

		case 'q':
			// throw an invalid number
			// so that it doesn't show
			// the message
			*lvlptr = -1;
			endgame();
			break;
	}
}

// we lost, what's next?
void gameover(void) {
	// play that cool transition tho'
	transition(T_DEBRIS);

	// and here's the menu itself
	printart(&over, 16, 0);
	mvwprintw(game, 54/2, (80-27)/2, "Mission failed at level %02d!", level);
	mvwprintw(game, 58/2, (80-16)/2, "Try again? [Y/N]");
	wrefresh(game);

	while (player.win == NULL) { // loop it
		key = wgetch(game);
		switch(key) {
			case 'y':
				// just restore the stats and start a
				// new game like nothing had happened
				player.win = genspr(player);
				player.hp  = 4;
				newlvl();
				break;

			case 'n': // or end it all
				endgame();
				break;

			default: // or ignore it in case if it's invalid
				continue;
				break;
		}
	}
}

// end cleanup
void endgame(void) {
	// gracefully end with a transition
	transition(T_DEBRIS);

	// finish curses now that the player's done
	endwin();

	// and tell them where they stopped, really not sure
	// how useful or cool this is
	if (level > 0) printf("Quit at level %d··· see you next time!\n", level);
	else if (level < 0) ;
	else printf("You won, thank you for playing!\n");

	// and we're done, hopefully everything went well
	// otherwise returning 0 would be embarrassing
	exit(0);
}

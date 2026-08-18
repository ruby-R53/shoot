#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "art.h"
#include "shoot.h"

// still nothing for this one, for now
WINDOW* game;

// but initialize the input buffer
int key = 0;

// and the level information
unsigned char level  = 1;
unsigned char levels = LVL_MAX;

// create a window with
// h height,
// w width, at
// y location,
// and x location,
// with or without a border!
WINDOW* create_win(int h, int w, int y, int x, bool border) {
	WINDOW* win;
	win = newwin(h, w, y, x);
	if (border) box(win, 0, 0);
	wrefresh(win); // and show it

	// and finally return it to the
	// variable it belongs to
	return win;
}

// generate sprite from character data
WINDOW* newspr(SPRITE spr) {
	// make it a derived window so that ncurses understands it
	spr.win = derwin(game, spr.h, spr.w, spr.y, spr.x);
	wprintw(spr.win, "%s", spr.skin);
	wrefresh(spr.win); // and show it

	// ditto
	return spr.win;
}

// move it
void mvspr(SPRITE spr) {
	werase(spr.win); // first, delete its trail
	mvderwin(spr.win, spr.y, spr.x); // before it, the sprite, can actually move

	// now actually show the thing at its specific
	// coordinates
	wprintw(spr.win, "%s", spr.skin);
	wtouchln(game, spr.y, spr.h, 1);
	// ^ but the parent window must be made aware of that first
	wnoutrefresh(game);
	doupdate(); // then actually update everything
}

// what the game is about, we need to specify
// where we're shooting from and where the
// opponent is
unsigned char shoot(SPRITE src, SPRITE dst) {
	// account the hitboxes for their
	// locations on screen
	src.hit[0] += src.y;
	src.hit[1] += src.x;
	dst.hit[0] += dst.y;
	dst.hit[1] += dst.x;

	// use src's hitbox as the
	// bullet's origin
	bullet.y = src.hit[0];
	bullet.x = src.hit[1];

	// spawn the bullet already
	bullet.win = newspr(bullet);

	// check if the player is shooting to flip
	// the bullet's direction (subtract from its
	// position), otherwise don't do anything
	char add = (src.win == player.win) ? -1 : 1;

	// now move the bullet up or down,
	// depending on who's shooting!
	do {
		bullet.y += add;
		mvspr(bullet);

		// if the target (dst) is around, check if the bullet
		// hit its hitbox (its y position and horizontal center)
		if (bullet.y == dst.hit[0] &&
			bullet.x == dst.hit[1]) {
			--dst.hp;
			health(dst);
			if (dst.hp == 0) kill(dst);
			break;
			// it makes sense that it disappears after hitting
			// something before the wall tho'
		}

		napms(5); // and move it every .005 secs
		flushinp(); // discard any input so that the game doesn't lag
	} while (bullet.y > 1 && bullet.y < 48);

	werase(bullet.win); // make it disappear!
	delwin(bullet.win); // let curses know it disappeared!
	if (dst.hp > 0) mvspr(dst);
	// ^ and redraw the thing in case it got hit but is still alive

	return dst.hp; // and that will be kept track of somewhere
}

// if someone got killed, make ncurses actually kill them too
void kill(SPRITE spr) {
	werase(spr.win); // erase the target
	wrefresh(spr.win);
	delwin(spr.win); // make curses know the target is dead
}

// health status for each sprite
void health(SPRITE spr) {
	if (spr.hp == 0) { // show a message to the player
		mvwprintw(spr.hud, 0, ((getmaxx(spr.hud)-7)/2), "Killed!");
		wrefresh(spr.hud);
		napms(125); // for .125 seconds
		werase(spr.hud);
	} else // pad it with zeros, i want it to be higher at a later point
		mvwprintw(spr.hud, 0, ((getmaxx(spr.hud)-6)/2), "HP: %02u", spr.hp);

	wrefresh(spr.hud);
}

// play a cool little transition between
// levels and moments
void transition(trans_t transition) {
	unsigned int y = 0, x = 0, tick = 0;
	const chtype fill[2] = { '.', ' ' };
	// y, x, and a loop count tracker

	switch(transition) {
		// wipe the screen with columnfuls of dots
		// then erase them column by column
		case T_CURTAIN:
			for (tick = 0; tick <= 1; ++tick) {
				for (x = 1; x <= 78; ++x) {
					for (y = 1; y <= 48; ++y)
						mvwaddch(game, y, x, fill[tick]);

					wrefresh(game);
					// so that it immediately gets
					// to the next part
					if (x < 78) napms(10);
				}
				// stop to show a cool message,
				// making sure the background
				// is still all dots
				if (tick == 0) {
					if (level <= levels)
						mvwprintw(game, 50/2, (80-22)/2, "Battle level %02u START!", level);
					else
						mvwprintw(game, 50/2, (80-16)/2, "Congratulations!");

					wrefresh(game);
					napms(500); // for .5 seconds
				}
			}
			break;

		// have a bunch of dots do a little square
		// movement to get erased after
		case T_DOTSYNC:
			for (int stage = 0; stage <= 1; ++stage) {
				for (tick = 0; tick <= 1; ++tick) {
					for (y = 1; y <= 48; y += 2) {
						for (x = 1+tick; x <= 78; x += 2)
							mvwaddch(game, y, x, fill[stage]);
					}
					wrefresh(game);
					napms(75);
				}
				for (tick = 0; tick <= 1; ++tick) {
					for (y = 2; y <= 48; y += 2) {
						for (x = 2-tick; x <= 78; x += 2)
							mvwaddch(game, y, x, fill[stage]);
					}
					wrefresh(game);
					napms(75);
				}
			}
			break;

		// or fill the screen with dots, clearing
		// them out one by one later
		case T_DEBRIS:
			tick = 2000;
			// 1k iterations (for each) seems reasonable,
			// it doesn't take very long to happen and it
			// doesn't fill the screen so much

			unsigned short backup[2][1000];
			unsigned short backpos = 0;
			// for smoothing the transition out, these will
			// store each of y and x's positions

			// initialize the RNG
			srandom(time(NULL));

			// first part, fill
			while (tick > 1000) {
				--tick;

				// choose a random spot to fill,
				// with the walls in mind
				y = 1 + random() % 48;
				x = 1 + random() % 78;

				// the screen filler is a dot, like
				// some kind of debris
				mvwaddch(game, y, x, '.');
				wrefresh(game);
				usleep(500); // .5 milliseconds for each

				// transfer current iteration to the
				// array
				backpos = (2000-1) - tick;

				// and store current positions for
				// later
				backup[0][backpos] = y;
				backup[1][backpos] = x;
				// one dot printed, 999 more to go
			}

			// second part, unfill
			while (tick > 0) {
				--tick;

				// restore the array indexer
				backpos = (1000-1) - tick;

				// so that we can unfill the spots back
				mvwaddch(game, backup[0][backpos], backup[1][backpos], ' ');
				wrefresh(game);
				usleep(500);
				// one dot removed, 999 more to go
			}

			// and finally clear the remaining mess in case
			// any message got printed as well
			werase(game);
			box(game, 0, 0); // which means redrawing the box
			break;
	}
}

// the enemy's movements, driven by RNG
void enemctrl(void) {
	/* boundary system that works
	 * following the above struct:
	 * the boundaries for shooting and
	 * for going up/down/left/right,
	 * all respectively */
	unsigned char bound[5] = {
		player.y, // for shooting based on the player's position*
		enemy.h, // upper boundary
		(49 - enemy.h), // lower boundary
		(enemy.w / 2) - (enemy.w % 2), // leftmost boundary**
		(79 - enemy.w) // rightmost boundary
		/* *do not shoot if the player is out of (vertical) reach!
		 * **the 2nd part is somewhat required because having an
		 * even-numbered width makes the 1st's calculation yield
		 * the actual boundary for that side of the screen, but
		 * then that isn't the case with odd-numbered ones */
	};

	// mix the current UNIX time with the previous
	// keypress for added unpredictability
	srandom((time(NULL)*key));
	move_t move = random() % 6; // limit that to the 5 available moves
	// now handle what our dice showed
	switch(move) {
		case MV_SHOOT:
			if (enemy.y < bound[0]) player.hp = shoot(enemy, player);
			break;

		case MV_UP:
			if (enemy.y > bound[1]) --enemy.y;
			break;

		case MV_DOWN:
			if (enemy.y < bound[2]) ++enemy.y;
			break;

		case MV_LEFT:
			if (enemy.x > bound[3]) --enemy.x;
			break;

		case MV_RIGHT:
			if (enemy.x < bound[4]) ++enemy.x;
			break;

		default: break;
	}

	// and finally, update the sprite
	mvspr(enemy);
}

// what to do on a new level
void newlvl(void) {
	if (level <= levels) {
		transition(T_CURTAIN); // play a cool transition

		// regenerate the enemy sprites,
		// along with an upgraded HP
		// and new positions
		srandom(time(NULL));
		enemy.y   = enemy.h + random() % 45 + 1;
		enemy.x   = (enemy.w / 2) + random() % (78 - enemy.w);
		// ^ +1 for the window boundaries (borders)
		enemy.hp  = level + 5;
		enemy.win = newspr(enemy);

		// and +1 the player's HP at
		// every 2 levels
		player.hp += level % 2;

		// display both health meters
		health(enemy);
		health(player);

		// because of the transition animation,
		// the screen got cleared, so redraw
		// the player too
		mvspr(player);
	}

	// and finally, update the counter
	counter();
}

// level counter
void counter(void) {
	if (level <= levels)
		mvwprintw(player.hud, 1, 0, "Level: %02u/%02u", level, levels);
	else { // you cleared the game!
		wclrtoeol(player.hud);
		mvwprintw(player.hud, 1, 0, " All Clear! ");
	}

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
	while ((key = wgetch(game))) {
		switch(key) {
			case 'z': // start the game
				return;
				break;

			case 'q': // or abort it
				endgame(true);
				exit(0);
				break;

			default: // or do nothing if it's invalid
				continue;
				break;
		}
	}
}

// we lost, what's next?
void gameover(void) {
	// play that cool transition tho'
	transition(T_DEBRIS);

	// and here's the menu itself
	printart(&over, 16, 0);
	mvwprintw(game, 54/2, (80-27)/2, "Mission failed at level %02u!", level);
	mvwprintw(game, 58/2, (80-16)/2, "Try again? [Y/N]");
	wrefresh(game);

	while (player.win == NULL) { // loop it
		key = wgetch(game);
		switch(key) {
			case 'y':
				// restore the player
				player.win = newspr(player);
				player.hp  = 4; // initial HP
				// the bonus accumulated by the player
				// up until the very point where they died
				for (int extra = 1; extra <= level; ++extra)
					player.hp += extra % 2;

				// and finally, restart the level
				newlvl();
				break;

			case 'n': // or end it all
				endgame(false);
				exit(0);
				break;

			default: // or ignore it in case if it's invalid
				continue;
				break;
		}
	}
}

// when the game is finally beaten
void ending(void) {
	flushinp(); // flush residual input just in case
	nodelay(game, FALSE);
	// ^ and disable the delay so that the screen
	// doesn't automatically get skipped

	// a cool variation of the stage clear
	// transition is played
	transition(T_CURTAIN);

	// the ending art is displayed, more
	// should be added to it i think
	printart(&won, 0, 0);

	// and the bottom text in italics
	// 'cos why the hell not
	wattron(game, A_ITALIC);
	mvwprintw(game, 45, (80-28)/2, "Press any key to continue···");
	wrefresh(game);
	wattroff(game, A_ITALIC);

	wgetch(game); // any key will do, really
}

// end cleanup
void endgame(bool aborted) {
	// gracefully end with a transition
	if (aborted || level > levels) transition(T_DOTSYNC);
	else transition(T_DEBRIS);

	// and finish curses now that the player's done
	delwin(game);
	endwin();

	// and tell them where they stopped, really not sure
	// how useful or cool this is
	if (!aborted) {
		if (level <= levels)
			printf("Quit at level %u··· see you next time!\n", level);
		else
			printf("You won, thank you for playing!\n");
	}
}

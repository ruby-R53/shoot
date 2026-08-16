#include <stdlib.h>
#include <unistd.h>
#include "shoot.h"

// where everything goes down
void ingame(void);

// where everything is stitched together
// to actually work
int main(int argc, char** argv) {
	// check if we have any argument to
	// change the amount of levels we want
	if (argc > 1) levels = atoi(argv[1]);
	if (levels == 0) levels = LVL_MAX; // 0 being atoi()'s error return value
	// otherwise just use the default of 12

	// initiate our screen, but
	// run some checks first
	initscr();

	// get the
	int ymax = getmaxy(stdscr); // tallest the screen can be, and
	int xmax = getmaxx(stdscr); // the widest

	// there has to be enough to fit our game in, 53x85 chars
	if (ymax <= 53 || xmax <= 85) {
		endwin();
		printf("%s\n%s\n",
				"Your terminal isn't at least 53x85 characters!",
				"Please resize your window and try again.");
		exit(1);
	}

	noecho(); // do NOT show the user's keystrokes here
	curs_set(0); // nor the cursor, this is a game after all
	raw(); // and don't do any input buffering

	// finally, render where the game itself is shown
	game = create_win(50, 80, (ymax-50)/2, (xmax-80)/2, true);

	// tell curses not to perform any extra cursor
	// movement
	leaveok(game, TRUE);

	// which's where we'll first show the title screen
	// for a cool little boot
	titlescr();

	// and now the HUDs, just a little below the main window
	player.hud = create_win(2, 12, (ymax+50)/2, (xmax-12)/2, false);
	enemy.hud  = create_win(1, 07, (ymax-52)/2, (xmax-07)/2, false);

	// then get to the game itself
	ingame();

	// and once the above is done, we
	// are done
	endgame(false);
	return 0;
}

// where the actual game happens
void ingame(void) {
	// start with a curtain-kind
	// of transition
	transition(T_CURTAIN);

	// create sprites for the
	player.win = newspr(player); // player
	enemy.win  = newspr(enemy); // and opponent

	const unsigned char bound[5] = {
		player.h, // upper boundary
		(49 - player.h), // lower boundary
		(player.w / 2) - (player.w % 2), // leftmost boundary
		(79 - enemy.w), // rightmost boundary
		player.h // shooting boundary
	};

	// show their respective HPs
	health(player);
	health(enemy);

	// and the level display
	counter();

	keypad(game, TRUE); // support for arrow keys
	// now, the main loop
	while (level <= levels) {
		key = wgetch(game);
		switch(key) {
			// yes, you can use Vim keys here
			case KEY_UP:
			case 'k':
				if (player.y > bound[0]) --player.y;
				break;

			case KEY_DOWN:
			case 'j':
				if (player.y < bound[1]) ++player.y;
				break;

			case KEY_LEFT:
			case 'h':
				if (player.x > bound[2]) --player.x;
				break;

			case KEY_RIGHT:
			case 'l':
				if (player.x < bound[3]) ++player.x;
				break;

			case 'z': // 'z' for shooting!
				if (player.y > bound[4])
					enemy.hp = shoot(player, enemy);
					// ^ this function not only shoots but also kills, so
					// the enemy's HP is tracked here as well

				// if we killed our opponent···
				if (enemy.hp == 0) {
					enemy.win = NULL; // end the sprite
					++level;
					newlvl();
					// ^ and create it again, on a different
					// level with new stats
				}
				// if not, try again!
				break;

			case 'x': // 'x' to skip your turn!
				break;

			case 'q': // 'q' exits the game!
				// now get back to main()
				return;
				break;

			default:
				continue; // do literally nothing whatsoever
				break;
		}

		// if the player has already
		// finished the game, skip all
		// of this!
		if (level > levels) break;

		// update the sprite
		mvspr(player);

		// and now it's the enemy's turn!
		enemctrl();

		// what to do whenever the player dies:
		// NULL their sprite's window,
		// and go to a nice little menu
		if (player.hp == 0) {
			player.win = NULL;
			gameover();
		}
	}

	// delete the player's window for
	// a clean finish
	delwin(player.win);

	// do a little cleanup by
	// closing the HUDs
	delwin(enemy.hud);
	delwin(player.hud);

	// and go to the end screen
	ending();
}

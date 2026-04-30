#include <stdlib.h>
#include <unistd.h>
#include "art.h"
#include "shoot.h"

// where the game cutely initializes
void titlescr(void);

// where everything goes down
int ingame(void);

// where everything is stitched together
// to actually work
void main(void) {
	// initiate our screen, but
	// run some checks first
	initscr();

	// get the
	int ymax = getmaxy(stdscr); // tallest the screen can be, and
	int xmax = getmaxx(stdscr); // the widest

	// there has to be enough to fit our game in tho', 85x53 chars
	if (ymax < 53 || xmax < 85) {
		endwin();
		printf("%s\n%s\n",
				"Your terminal isn't at least 85x53 characters!",
				"Please try resizing your window and try again.");
		exit(1);
	}

	nodelay(game, TRUE); // to make wgetch() non-blocking
	noecho(); // do NOT show the user's keystrokes here
	curs_set(0); // nor the cursor, this is a game after all
	raw(); // and don't do any input buffering

	// finally, render where the
	game = create_win(50, 80, (ymax-50)/2, (xmax-80)/2, true); // game itself is shown

	// which's where we'll first show the title screen
	// for a cool little boot
	titlescr();

	// and now its HUDs
	player.hud = create_win(2, 9, (ymax+50)/2, (xmax-9)/2, false);
	enemy.hud  = create_win(1, 7, (ymax-52)/2, (xmax-7)/2, false);

	keypad(game, TRUE); // support for arrow keys

	// create sprites for the
	player.win = genspr(player); // player
	enemy.win  = genspr(enemy); // and opponent, yet to be further programmed

	// keep track of what level we left at
	// to show the user some message
	unsigned int finish = ingame();

	// and once the above returns, we're done
	endgame(finish);
}

// how the title screen is displayed
void titlescr(void) {
	// play a cool little animation for the game title
	printart(&title, (50-title.rows)/2, (80-title.cols)/2);

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
			transition();
			return;
			break;

		case 'q':
			endgame(-1);
			break;
	}
}

// and this is where the actual game happens!
int ingame(void) {
	int level = 1; // start at level 1 'cos 0 sounds too nerdy (???)
	enemy.hp  = 6; // higher than the player's but not by much

	// show their respective HPs
	health(player);
	health(enemy);

	goto counter; // ugh i hate calling the same thing twice

	// now, the main loop
	while (level <= LVL_MAX) {
		key = wgetch(game);
		switch(key) {
			// yes, you can use Vim keys here
			case KEY_UP:
			case 'k':
				if (player.y >= 2) movespr(player, --player.y, player.x);
				break;

			case KEY_DOWN:
			case 'j':
				if (player.y <= 47) movespr(player, ++player.y, player.x);
				break;

			case KEY_LEFT:
			case 'h':
				if (player.x >= 2) movespr(player, player.y, --player.x);
				break;

			case KEY_RIGHT:
			case 'l':
				if (player.x <= 73) movespr(player, player.y, ++player.x);
				break;

			// 2, 47, 2, 73··· these are all the player's boundaries (Y, Y, X, X), so
			// that they don't straight-up get thru the walls here

			case 'z': // 'z' for shooting!
				if (player.y > 2) // but why would you shoot the wall brah
					// this function not only shoots but also kills, so
					// the enemy's HP is tracked here as well
					enemy.hp = shoot(player, enemy);

				// if we killed our opponent···
				if (enemy.hp == 0 && enemy.win != NULL) {
					enemy.win = NULL; // end the sprite
					newlvl(++level);
					// ^ but create it again, on a different
					// level with new stats
				}
				// if not, try again!
				break;

			case 'q': // 'q' exits the game!
				// do a little cleanup by
				// closing the HUDs
				delwin(enemy.hud);
				delwin(player.hud);
				// now get back to main()
				return level;
				break;
		}

		// it's the enemy's turn!
		enemctrl();

		// what to do whenever the player dies
		if (player.hp == 0) {
			player.win = NULL;
			gameover(level);
		}

counter:
		mvwprintw(player.hud, 1, 0, "Level: %02d", level);
		wrefresh(player.hud);
	}

	// this is only executed when we win, since the loop
	// actually ends when its condition is no longer met
	return 0;
}

#include <stdlib.h>
#include <unistd.h>
#include "shoot.h"

void bombstatus(int bombs);

int main(void) {
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

	nodelay(game, TRUE); // to make getch() non-blocking
	noecho(); // do NOT show the user's keystrokes here
	curs_set(0); // nor the cursor, this is a game after all
	raw(); // and don't do any input buffering

	// finally, render where the
	game = create_win(50, 80, (ymax-50)/2, (xmax-80)/2, true); // game itself is shown
	hud.top    = create_win(1, 7, (ymax-52)/2, (xmax-7)/2, false);
	hud.bottom = create_win(2, 20, (ymax+50)/2, (xmax-20)/2, false);
	// ^ and its HUDs

	keypad(game, TRUE); // support for arrow keys

	// create sprites for the
	player.win = genspr(player); // player
	enemy.win  = genspr(enemy); // and opponent, yet to be further programmed

	health(player, hud.bottom);
	health(enemy, hud.top);

	int bombs = 3; // this clears EVERYTHING on the screen so use it as a last resort
	bombstatus(bombs); // show that to the user

	int key = 0; // this is what's gonna carry what getch() gets
	
	// now, the main loop
	while ((key = wgetch(game)) != 'q') { // 'q' exits the game!
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
					// feed what it returns to the kill tracker
					enemy.hp = shoot(player, enemy);

				if (enemy.hp == 0 && enemy.win != NULL) { // if we killed the opponent···
					enemy.win = NULL; // tell shoot() our enemy is dead
				} // if not, try again!
				break;

			case 'x': // 'x' for bombing!
				if (bombs > 0) {
					--bombs;
					bombstatus(bombs);
					boom(player);
					// these arguments must be passed so that after the screen gets
					// cleared, the player's sprite can be redrawn
				}
				break;
		}
	}

	delwin(hud.top); // FIXME
	delwin(hud.bottom);
	endwin();
	return 0;
}

void bombstatus(int bombs) { // bomb tracker updater
	mvwprintw(hud.bottom, 1, 0, "Available Bombs: %d", bombs);
	wrefresh(hud.bottom);
}

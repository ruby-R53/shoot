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
		printf("Your terminal isn't at least 85x53 characters!\n");
		printf("Please try resizing your window and try again.\n");
		exit(1);
	}

	nodelay(game, TRUE); // to make getch() non-blocking
	noecho(); // do NOT show the user's keystrokes here
	curs_set(0); // nor the cursor, this is a game after all
	raw(); // and don't do any input buffering

	// finally, render where the
	game = create_win(50, 80, (ymax-50)/2, (xmax-80)/2, true); // game itself is displayed
	hud  = create_win(3, 20, (ymax+52)/2, (xmax-20)/2, false);
	// and its HUD, just a little (2 rows) below it

	keypad(game, TRUE); // so that we can use arrow keys here

	// create sprites for the
	WINDOW* player_w = genspr(player); // player
	WINDOW* enemy_w  = genspr(enemy); // and opponent, yet to be further programmed

	int bombs = 3; // this clears EVERYTHING on the screen but use it as a last resort
	bombstatus(bombs); // show that to the user

	int key = 0; // this is what's gonna carry what getch() gets
	bool kill = false; // we haven't killed anyone just yet
	
	// now, the main loop
	while ((key = wgetch(game)) != 'q') { // 'q' exits the game!
		switch(key) {
			// yes, you can use Vim keys here
			case KEY_UP:
			case 'k':
				if (player.y >= 2) movespr(player_w, player.skin, --player.y, player.x);
				break;

			case KEY_DOWN:
			case 'j':
				if (player.y <= 47) movespr(player_w, player.skin, ++player.y, player.x);
				break;

			case KEY_LEFT:
			case 'h':
				if (player.x >= 2) movespr(player_w, player.skin, player.y, --player.x);
				break;

			case KEY_RIGHT:
			case 'l':
				if (player.x <= 73) movespr(player_w, player.skin, player.y, ++player.x);
				break;

			// 2, 47, 2, 73··· these are all the player's boundaries (Y, Y, X, X), so
			// that they don't straight-up get thru the walls here

			case 'z': // 'z' for shooting!
				if (player.y > 2) // but why would you shoot the wall brah
					// this function not only shoots but also kills, so
					// feed what it returns to the kill tracker
					kill = shoot(enemy_w, player, enemy);

				if (kill) { // if we killed the opponent···
					werase(hud);
					wprintw(hud, "\rKilled!"); // show it on the HUD
					wrefresh(hud);
					usleep(250000); // for .25 seconds
					kill = false;
					enemy_w = NULL; // tell shoot() our enemy is dead
					bombstatus(bombs); // and go back to the previous state
				} // if not, try again!
				break;

			case 'x': // 'x' for bombing!
				if (bombs > 0) {
					--bombs;
					wprintw(hud, "\rAvailable Bombs: %d", bombs);
					wrefresh(hud);
					boom(player_w, player);
					// these arguments must be passed so that after the screen gets
					// cleared, the player's sprite can be redrawn
				}
				break;
		}
	}

	delwin(hud); // FIXME
	endwin();
	return 0;
}

void bombstatus(int bombs) { // bomb tracker updater
	wprintw(hud, "\rAvailable Bombs: %d", bombs);
	wrefresh(hud);
}

#include <stdlib.h>
#include "shoot.h"

int main() {
	initscr();

	int ymax = 0;
	int xmax = 0;

	getmaxyx(stdscr, ymax, xmax);

	if (ymax < 52 || xmax < 85) {
		endwin();
		printf("Your terminal isn't at least 85x52 characters!\n");
		printf("Please try resizing your window and try again.\n");
		exit(1);
	}

	nodelay(game, TRUE);
	noecho();
	curs_set(0);
	raw();

	int bombs = 3;

	hud = create_win(3, 20, (ymax+52)/2, (xmax-20)/2, false);
	wprintw(hud, "Available Bombs: %d", bombs);
	wrefresh(hud);
	game = create_win(50, 80, (ymax-50)/2, (xmax-80)/2, true);

	keypad(game, TRUE);

	WINDOW* player_w = genspr(player);
	WINDOW* enemy_w  = genspr(enemy);

	int key = 0;
	while ((key = wgetch(game)) != 'q') {
		switch(key) {
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

			case 'z':
				bool kill = shoot(enemy_w, player, enemy);
				if (kill == true) {
					wprintw(hud, "\rKilled!");
					wrefresh(hud);
				}
				break;

			case 'x':
				if (bombs > 0) {
					--bombs;
					wprintw(hud, "\rAvailable Bombs: %d", bombs);
					wrefresh(hud);
					boom(player_w, player);
				}
				break;
		}
	}

	endwin();
	return 0;
}

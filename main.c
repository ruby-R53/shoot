#include <stdlib.h>
#include <unistd.h>
#include "shoot.h"

void bombstatus(int bombs);

int main() {
	initscr();

	int ymax = getmaxy(stdscr);
	int xmax = getmaxx(stdscr);

	if (ymax < 53 || xmax < 85) {
		endwin();
		printf("Your terminal isn't at least 85x53 characters!\n");
		printf("Please try resizing your window and try again.\n");
		exit(1);
	}

	nodelay(game, TRUE);
	noecho();
	curs_set(0);
	raw();

	game = create_win(50, 80, (ymax-50)/2, (xmax-80)/2, true);
	hud  = create_win(3, 20, (ymax+52)/2, (xmax-20)/2, false);

	keypad(game, TRUE);

	WINDOW* player_w = genspr(player);
	WINDOW* enemy_w  = genspr(enemy);

	int bombs = 3;
	bombstatus(bombs);

	int key = 0;
	bool kill = true;
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
				if (player.y > 2) kill = shoot(enemy_w, player, enemy);
				if (kill) {
					werase(hud);
					wprintw(hud, "\rKilled!");
					wrefresh(hud);
					usleep(250000);
					kill = false;
					bombstatus(bombs);
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

	delwin(hud);
	endwin();
	return 0;
}

void bombstatus(int bombs) {
	wprintw(hud, "\rAvailable Bombs: %d", bombs);
	wrefresh(hud);
}

#include <stdlib.h>
#include <unistd.h>
#include "shoot.h"

// where everything goes down
void ingame(void);

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
				"Please resize your window and try again.");
		exit(1);
	}

	noecho(); // do NOT show the user's keystrokes here
	curs_set(0); // nor the cursor, this is a game after all
	raw(); // and don't do any input buffering

	// finally, render where the
	game = create_win(50, 80, (ymax-50)/2, (xmax-80)/2, true); // game itself is shown
	nodelay(game, TRUE); // to make wgetch() non-blocking

	// which's where we'll first show the title screen
	// for a cool little boot
	titlescr();

	// and now its HUDs
	player.hud = create_win(2, 10, (ymax+50)/2, (xmax-9)/2, false);
	enemy.hud  = create_win(1, 7, (ymax-52)/2, (xmax-7)/2, false);

	keypad(game, TRUE); // support for arrow keys

	// finally get to the game itself
	ingame();

	// and once the above is done, we
	// are done
	endgame();
}

void render_sprite(SPRITE spr) {
	mvwprintw(game, spr.y, spr.x, "%s", spr.skin);
}

void render() {
	werase(game);

	render_sprite(player);
	render_sprite(enemy);
	if (isshooting)
		render_sprite(bullet);
	
	box(game, 0, 0); // box has to be redrawn after window is cleared
	doupdate();
}

// and this is where the actual game happens!
void ingame(void) {
	enemy.hp = 6; // higher than the player's but not by much

	// show their respective HPs
	health(&player);
	health(&enemy);

	// and the level display
	counter();

	// now, the main loop
	while (level <= LVL_MAX) {
		key = wgetch(game);
		switch(key) {
			// yes, you can use Vim keys here
			case KEY_UP:
			case 'k':
				if (isplayersturn && !isshooting && player.y >= 2) --player.y;
				isplayersturn = false;
				break;

			case KEY_DOWN:
			case 'j':
				if (isplayersturn && !isshooting && player.y <= 47) ++player.y;
				isplayersturn = false;
				break;

			case KEY_LEFT:
			case 'h':
				if (isplayersturn && !isshooting && player.x >= 2) --player.x;
				isplayersturn = false;
				break;

			case KEY_RIGHT:
			case 'l':
				if (isplayersturn && !isshooting && player.x <= 73) ++player.x;
				isplayersturn = false;
				break;

			// 2, 47, 2, 73··· these are all the player's boundaries (Y, Y, X, X), so
			// that they don't straight-up get thru the walls here

			case 'z': // 'z' for shooting!
				if (isplayersturn && !isshooting && player.y > 2) { // but why would you shoot the wall brah
					// this function not only shoots but also kills, so
					// the enemy's HP is tracked here as well
					shootbullet(&player, &enemy);
				}
				
				isplayersturn = false;
				break;

			case 'x': // 'x' to skip your turn!
				isplayersturn = false;
				break;

			case 'q': // 'q' exits the game!
				// do a little cleanup by
				// closing the HUDs
				delwin(enemy.hud);
				delwin(player.hud);
				// now get back to main()
				return;
				break;

			default:
				break;
		}

		// it's the enemy's turn!
		if (!isplayersturn) {
			enemctrl();
			isplayersturn = true;
		}

		if (isshooting)
			movebullet();

		render();

		// what to do whenever the player dies
		if (player.hp == 0) {
			gameover();
		}
	}

	// when you "clear" the game, you're
	// actually doing exactly that ;)
	*lvlptr = 0;
}

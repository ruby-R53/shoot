#include <unistd.h>
#include "art.h"

// the title screen!
ART title = {
	.art = {
		" ::::::::  :::    :::  ::::::::   :::::::: ::::::::::: :::",
		":+:    :+: :+:    :+: :+:    :+: :+:    :+:    :+:     :+:",
		"+:+        +:+    +:+ +:+    +:+ +:+    +:+    +:+     +:+",
		"+#++:++#++ +#++:++#++ +#+    +:+ +#+    +:+    +#+     +#+",
		"       +#+ +#+    +#+ +#+    +#+ +#+    +#+    +#+     +#+",
		"#+#    #+# #+#    #+# #+#    #+# #+#    #+#    #+#        ",
		" ########  ###    ###  ########   ########     ###     ###",
	},
	.rows = 7,
	.cols = 58
};

// and game over one!
ART over = {
	.art = {
		" ####    ##   #    # ######     ####  #    # ###### #####  ",
		"#    #  #  #  ##  ## #         #    # #    # #      #    # ",
 		"#      #    # # ## # #####     #    # #    # #####  #    # ",
		"#  ### ###### #    # #         #    # #    # #      #####  ",
		"#    # #    # #    # #         #    #  #  #  #      #   #  ",
		" ####  #    # #    # ######     ####    ##   ###### #    # ",
	},
	.rows = 6,
	.cols = 59
};

// and a function for how they'll be displayed
void printart(ART* art, int y, int x) {
	// default position: the game window's center
	if (y < 1) y = (50-art->rows)/2;
	if (x < 1) x = (80-art->cols)/2;

	// a window might not be necessary for now
	// but in case i ever decide to implement
	// colors it'll end up being useful
	art->win = derwin(game, art->rows, art->cols, y, x);

	// write each row into the screen with a little
	// delay so that it's animated
	for (int i = 0; i <= art->rows; ++i) {
		mvwprintw(art->win, i, 0, "%s", art->art[i]);
		touchwin(game);
		wrefresh(art->win);
		usleep(50000);
	}

	// and delete it while leaving its
	// contents onscreen
	delwin(art->win);
}

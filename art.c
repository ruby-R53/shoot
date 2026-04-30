#include "art.h"
#include <unistd.h>

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

void printart(ART* art, int y, int x) {
	// a window might not be necessary for now
	// but in case i ever decide to implement
	// colors it'll end up being useful
	art->win = derwin(game, title.rows, title.cols, y, x);

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

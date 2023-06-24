#include "libce.h"
#include "vid.h"

using namespace vid;

void char_write(natb c) {
	switch (c) {
	case 0:
		break;
	case '\r':
		x = 0;
		break;
	case '\n':
		x = 0;
		y++;
		if (y >= ROWS)
			scroll();
		break;
	case '\b':
		if (x > 0 || y > 0) {
			if (x == 0) {
				x = COLS - 1;
				y--;
			} else {
				x--;
			}
		}
		break;
	default:
		video[y * COLS + x] = attr | c;
		x++;
		if (x >= COLS) {
			x = 0;
			y++;
		}
		if (y >= ROWS)
			scroll();
		break;
	}
	cursore();
}

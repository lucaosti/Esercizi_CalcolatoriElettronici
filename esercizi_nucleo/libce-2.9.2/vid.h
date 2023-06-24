#include "tipo.h"

namespace vid {

	const natl COLS = 80;
	const natl ROWS = 25;
	const natl VIDEO_SIZE = COLS * ROWS;
	const natb CUR_HIGH = 0x0e;
	const natb CUR_LOW = 0x0f;

	const ioaddr iIND = 0x03D4;
	const ioaddr iDAT = 0x03D5;

	extern volatile natw* video;
	extern natb x, y;
	extern natw attr;

}

void cursore();
void scroll();

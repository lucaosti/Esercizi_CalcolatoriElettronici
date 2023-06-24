#include "internal.h"
#include "kbd.h"

using namespace kbd;

char char_read_int()
{	
	natb c; char a;
	c = get_code_int();
	if (c==0x2A)
		shift = true;
	else if (c==0xAA)
		shift = false;
	if (c >= 0x80 || c == 0x2A)
		return 0;
	a = conv(c);
	return a;
}

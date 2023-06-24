#include "internal.h"
#include "kbd.h"

using namespace kbd;

char char_read()
{	
	natb c; char a;
	do
	{	c = get_code();
		if (c==0x2A) shift = true; 			// left shift make code 
			else if (c==0xAA)  shift = false;	// left shift break code	
	} while (c>=0x80 || c==0x2A);				// make code;
	a = conv(c);						// conv() puo' restituire 0
	return a;						// 0 se tasto non riconosciuto
}

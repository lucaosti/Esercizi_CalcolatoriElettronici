#include "libce.h"
#include "com1.h"

using namespace com1;

void serial_o(natb c)
{
	natb s;
	do {
		s = inputb(iLSR);
	} while (! (s & 0x20));
	outputb(c, iTHR);
}

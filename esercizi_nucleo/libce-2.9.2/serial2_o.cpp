#include "libce.h"
#include "com2.h"

using namespace com2;

void serial2_o(natb c)
{
	natb s;
	do {
		s = inputb(iLSR);
	} while (! (s & 0x20));
	outputb(c, iTHR);
}

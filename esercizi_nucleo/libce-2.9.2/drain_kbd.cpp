#include "libce.h"
#include "kbd.h"

using namespace kbd;

void drain_kbd()
{
	while (inputb(iSTR) & 0x01) {
		inputb(iRBR);
	}
}

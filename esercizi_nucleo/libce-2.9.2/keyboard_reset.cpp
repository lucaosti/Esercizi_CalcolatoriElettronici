#include "internal.h"
#include "kbd.h"

using namespace kbd;

void keyboard_reset()
{
	outputb(0x60, iCMR);
	outputb(0x61, iRBR);
}

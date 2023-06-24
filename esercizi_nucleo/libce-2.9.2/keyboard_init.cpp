#include "internal.h"
#include "kbd.h"

using namespace kbd;

void keyboard_init()
{
	// scrittura del byte di comando
	outputb(0x60, iCMR);
	outputb(0x60, iRBR);
}

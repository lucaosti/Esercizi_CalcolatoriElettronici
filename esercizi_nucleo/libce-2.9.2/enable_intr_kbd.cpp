#include "libce.h"
#include "kbd.h"

using namespace kbd;

void enable_intr_kbd()
{
	outputb(0x60, iCMR);
	outputb(0x61, iTBR);
}

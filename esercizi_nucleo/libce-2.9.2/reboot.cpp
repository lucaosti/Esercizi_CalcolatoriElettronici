#include "internal.h"
#include "kbd.h"

using namespace kbd;

void reboot()
{
	outputb(0xFE, iCMR);
	for (;;);
}

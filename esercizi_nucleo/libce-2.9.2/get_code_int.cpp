#include "internal.h"
#include "kbd.h"

using namespace kbd;

natb get_code_int()
{
	return inputb(iRBR);
}

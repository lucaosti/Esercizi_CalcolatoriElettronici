#include "libce.h"
#include "hd.h"

using namespace hd;

void hd_wait_for_br()
{
	natb s;
	do
		s = inputb(iSTS);
	while ((s & 0x88) != 0x08);
}

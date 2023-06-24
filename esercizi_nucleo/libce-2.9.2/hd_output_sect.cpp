#include "libce.h"
#include "hd.h"

using namespace hd;

void hd_output_sect(natb vetto[])
{
	hd_wait_for_br();
	outputbw(reinterpret_cast<natw*>(vetto), 256, iBR);
}

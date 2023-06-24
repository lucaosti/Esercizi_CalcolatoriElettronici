#include "libce.h"
#include "hd.h"

using namespace hd;

void hd_input_sect(natb vetto[])
{
	hd_wait_for_br();
	inputbw(iBR, reinterpret_cast<natw*>(vetto), 256);
}

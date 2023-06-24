#include "libce.h"
#include "hd.h"

using namespace hd;

void hd_ack_intr()
{
	inputb(iSTS);
}

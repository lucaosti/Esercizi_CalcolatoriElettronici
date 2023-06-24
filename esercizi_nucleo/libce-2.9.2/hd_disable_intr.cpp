#include "libce.h"
#include "hd.h"

using namespace hd;

void hd_disable_intr()
{
	outputb(0x02, iDCR);
}

#include "libce.h"
#include "hd.h"

using namespace hd;

void hd_enable_intr()
{
	outputb(0x00, iDCR);
}

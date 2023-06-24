#include "libce.h"
#include "hd.h"

using namespace hd;

void hd_set_lba(natl lba)
{
	natb lba_0 = lba,
	     lba_1 = lba >> 8,
	     lba_2 = lba >> 16,
	     lba_3 = lba >> 24;

	outputb(lba_0, iSNR);
	outputb(lba_1, iCNL);
	outputb(lba_2, iCNH);
	natb hnd = (lba_3 & 0x0F) | 0xE0;
	outputb(hnd, iHND);
}

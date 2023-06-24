#include "libce.h"
#include "hd.h"

using namespace hd;

void hd_start_cmd(natl lba, natb n, natb cmd)
{
	hd_set_lba(lba);
	outputb(n, iSCR);
	outputb(cmd, iCMD);
}

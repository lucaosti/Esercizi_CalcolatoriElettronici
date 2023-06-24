#include "libce.h"
#include "bm.h"

using namespace bm;

void bm_ack()
{
	natb work = inputb(iBMCMD);
	work &= 0xFE;
	outputb(work, iBMCMD);
	inputb(iBMSTR);
}

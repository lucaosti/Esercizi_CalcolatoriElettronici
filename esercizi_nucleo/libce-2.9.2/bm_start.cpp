#include "libce.h"
#include "bm.h"

using namespace bm;

void bm_start()
{
	natb work = inputb(iBMCMD);
	work |= 1;
	outputb(work, iBMCMD);
}

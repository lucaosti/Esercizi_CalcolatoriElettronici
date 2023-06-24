#include "libce.h"
#include "vm.h"


bool tab_iter::right()
{
	if (done() || sp()->cur == sp()->end)
		return false;

	sp()->cur += dim_region(l - 1);

	return sp()->cur != sp()->end;
}

#include "libce.h"
#include "vm.h"

bool tab_iter::up()
{
	if (done())
		return false;

	l++;
	return true;
}

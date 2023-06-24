#include "libce.h"
#include "vm.h"


void tab_iter::post()
{
	if (done())
		return;

	while (down())
		;
}

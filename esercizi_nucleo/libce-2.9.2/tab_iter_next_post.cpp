#include "libce.h"
#include "vm.h"


void tab_iter::next_post()
{
	if (done())
		return;

	if (right()) {
		post();
	} else {
		up();
	}
}

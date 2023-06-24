#include "libce.h"

void *operator new(size_t, align_val_t);
void *operator new[](size_t s, align_val_t a)
{
	return operator new(s, a);
}

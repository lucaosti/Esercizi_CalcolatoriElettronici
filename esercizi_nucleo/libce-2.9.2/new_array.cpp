#include "tipo.h"

void *operator new[](size_t s)
{
	return operator new(s);
}

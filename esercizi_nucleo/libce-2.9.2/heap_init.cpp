#include "internal.h"
#include "heap.h"

void heap_init(void *start, size_t size)
{
	free_interna(start, size);
}

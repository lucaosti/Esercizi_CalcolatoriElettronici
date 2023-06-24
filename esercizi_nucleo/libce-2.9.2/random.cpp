#include "internal.h"
#include "random.h"

using namespace rnd;

natl random()
{
	seed = (seed * 1664525UL) + 1013904223;
	return seed;
}

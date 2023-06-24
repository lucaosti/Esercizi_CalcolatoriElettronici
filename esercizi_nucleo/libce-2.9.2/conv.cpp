#include "internal.h"
#include "kbd.h"

using namespace kbd;

char conv(natb c) {
	char cc;
	natl pos = 0;
	while (pos < MAX_CODE && tab[pos] != c)
		pos++;
	if (pos == MAX_CODE)
		return 0;
	if (shift)
		cc = tabmai[pos];
	else
		cc = tabmin[pos];
	return cc;
}

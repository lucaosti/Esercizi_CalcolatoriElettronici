#include "tipo.h"

natq read_uleb128(vaddr& s)
{
	natq u = 0;
	natb b;
	int shift = 0;
	do {
		b = *reinterpret_cast<natb*>(s++);
		u |= (b & 0x7f) << shift;
		shift += 7;
	} while (b & 0x80);
	return u;
}

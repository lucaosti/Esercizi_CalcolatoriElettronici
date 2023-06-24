#include "internal.h"
#include "apic.h"

using namespace apic;

extern "C" natl apic_read_irr(natb i)
{
	if (i > 7)
		return 0;
        return *reinterpret_cast<volatile natl*>(0xfee00200 + i * 16);
}

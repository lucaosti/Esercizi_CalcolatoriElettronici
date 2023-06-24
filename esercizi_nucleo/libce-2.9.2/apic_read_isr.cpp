#include "internal.h"
#include "apic.h"

using namespace apic;

extern "C" natl apic_read_isr(natb i)
{
	if (i > 7)
		return 0;
        return *reinterpret_cast<volatile natl*>(0xfee00100 + i * 16);
}

#include "internal.h"
#include "apic.h"

using namespace apic;

void apic_write_rtl(natb irq, natl w)
{
        apic_out(RTO + irq * 2, w);
}

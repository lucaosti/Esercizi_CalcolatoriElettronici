#include "internal.h"
#include "apic.h"

using namespace apic;

void apic_write_rth(natb irq, natl w)
{
        apic_out(RTO + irq * 2 + 1, w);
}

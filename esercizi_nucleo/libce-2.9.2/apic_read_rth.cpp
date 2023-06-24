#include "internal.h"
#include "apic.h"

using namespace apic;

natl apic_read_rth(natb irq)
{
        return apic_in(RTO + irq * 2 + 1);
}

#include "internal.h"
#include "apic.h"

using namespace apic;

natl apic_read_rtl(natb irq)
{
        return apic_in(RTO + irq * 2);
}

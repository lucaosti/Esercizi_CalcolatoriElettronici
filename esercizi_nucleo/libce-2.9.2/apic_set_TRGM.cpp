#include "internal.h"
#include "apic.h"

using namespace apic;

void apic_set_TRGM(natl irq, bool v)
{
        natl work = apic_read_rtl(irq);
        if (v)
                work |= TRGM_BIT;
        else
                work &= ~TRGM_BIT;
        apic_write_rtl(irq, work);
}

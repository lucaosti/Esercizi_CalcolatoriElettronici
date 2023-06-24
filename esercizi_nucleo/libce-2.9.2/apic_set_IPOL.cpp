#include "internal.h"
#include "apic.h"

using namespace apic;

void apic_set_IPOL(natl irq, bool v)
{
        natl work = apic_read_rtl(irq);
        if (v)
                work |= IPOL_BIT;
        else
                work &= ~IPOL_BIT;
        apic_write_rtl(irq, work);
}

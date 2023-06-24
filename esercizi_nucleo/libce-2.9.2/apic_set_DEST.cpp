#include "internal.h"
#include "apic.h"

using namespace apic;

void apic_set_DEST(natl irq, natb dest)
{
        natl work = apic_read_rth(irq);
        work = (work & ~DEST_MSK) | (dest << DEST_SHF);
        apic_write_rth(irq, work);
}

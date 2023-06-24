#include "internal.h"
#include "apic.h"

using namespace apic;

void apic_reset()
{
        for (natb i = 0; i < IRQ_MAX; i++) {
                apic_write_rth(i, 0);
                apic_write_rtl(i, MIRQ_BIT | TRGM_BIT);
        }
        apic_set_TRGM(0, false);
        apic_set_TRGM(2, false);
}

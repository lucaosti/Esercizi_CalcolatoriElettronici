#include "internal.h"
#include "piix3.h"
#include "apic.h"

using namespace apic;
using namespace piix3;

bool piix3_ioapic_enable(natb bus, natb dev, natb fun)
{
        natl xbcs = pci_read_confl(bus, dev, fun, XBCS);
        xbcs |= XBCS_ENABLE;
        pci_write_confl(bus, dev, fun, XBCS, xbcs);
        return true;
}

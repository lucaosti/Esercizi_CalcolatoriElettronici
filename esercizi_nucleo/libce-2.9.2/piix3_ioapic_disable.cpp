#include "internal.h"
#include "piix3.h"

bool piix3_ioapic_disable(natb bus, natb dev, natb fun)
{
        natl xbcs = pci_read_confl(bus, dev, fun, piix3::XBCS);
        xbcs &= ~piix3::XBCS_ENABLE;
        pci_write_confl(bus, dev, fun, piix3::XBCS, xbcs);
        return true;
}

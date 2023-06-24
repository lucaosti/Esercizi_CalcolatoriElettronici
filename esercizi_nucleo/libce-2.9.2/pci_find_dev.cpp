#include "internal.h"
#include "pci.h"

bool pci_find_dev(natb& bus, natb& dev, natb& fun, natw devID, natw vendID)
{
	do {
		natw work;

		if ( (work = pci_read_confw(bus, dev, fun, 0)) == 0xFFFF ) 
			continue;
		if ( work == vendID && pci_read_confw(bus, dev, fun, 2) == devID) 
			return true;
	} while (pci_next(bus, dev, fun));
	return false;
}

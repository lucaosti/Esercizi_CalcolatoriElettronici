#include "internal.h"
#include "pci.h"

using namespace pci;

void pci_write_confb(natb bus, natb dev, natb fun, natb off, natb data)
{
	natl confaddr = make_CAP(bus, dev, fun, off);
	outputl(confaddr, CAP);
	outputb(data, CDP + (off & 0x03));
}

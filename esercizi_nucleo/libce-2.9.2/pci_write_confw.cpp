#include "internal.h"
#include "pci.h"

using namespace pci;

void pci_write_confw(natb bus, natb dev, natb fun, natb off, natw data)
{
	natl confaddr = make_CAP(bus, dev, fun, off);
	outputl(confaddr, CAP);
	outputw(data, CDP + (off & 0x03));
}

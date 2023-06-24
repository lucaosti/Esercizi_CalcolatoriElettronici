#include "internal.h"
#include "pci.h"

using namespace pci;

natw pci_read_confw(natb bus, natb dev, natb fun, natb off)
{
	natl confaddr = make_CAP(bus, dev, fun, off);
	outputl(confaddr, CAP);
	return inputw(CDP + (off & 0x03));
}

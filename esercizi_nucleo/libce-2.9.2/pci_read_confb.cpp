#include "internal.h"
#include "pci.h"

using namespace pci;

natb pci_read_confb(natb bus, natb dev, natb fun, natb off)
{
	natl confaddr = make_CAP(bus, dev, fun, off);
	outputl(confaddr, CAP);
	return inputb(CDP + (off & 0x03));
}

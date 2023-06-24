#include "internal.h"
#include "pci.h"

using namespace pci;

void pci_write_confl(natb bus, natb dev, natb fun, natb off, natl data)
{
	natl confaddr = make_CAP(bus, dev, fun, off);
	outputl(confaddr, CAP);
	outputl(data, CDP);
}

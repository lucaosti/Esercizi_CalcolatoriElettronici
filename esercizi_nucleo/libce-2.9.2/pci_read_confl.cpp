#include "internal.h"
#include "pci.h"

using namespace pci;

natl pci_read_confl(natb bus, natb dev, natb fun, natb off)
{
	natl confaddr = make_CAP(bus, dev, fun, off);
	outputl(confaddr, CAP);
	return inputl(CDP);
}

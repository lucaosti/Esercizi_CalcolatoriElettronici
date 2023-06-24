#include "libce.h"
#include "bm.h"

using namespace bm;

void bm_init(natb bus, natb dev, natb fun)
{
	natl base = pci_read_confl(bus, dev, fun, 0x20);
	base &= ~0x1;
	iBMCMD  = (ioaddr)(base + 0x00);
	iBMSTR  = (ioaddr)(base + 0x02);
	iBMDTPR = (ioaddr)(base + 0x04);
	natw cmd = pci_read_confw(bus, dev, fun, 4);
	pci_write_confw(bus, dev, fun, 4, cmd | 0x5);
}

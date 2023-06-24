#include "internal.h"
#include "pci.h"

bool pci_find_class(natb& bus, natb& dev, natb& fun, natb code[])
{
	do {
		if (pci_read_confw(bus, dev, fun, 0) == 0xFFFF)
			continue;
		natb work[3];
		natl i;
		for (i = 0; i < 3; i++) {
			work[i] = pci_read_confb(bus, dev, fun, 2 * 4 + i + 1);
			if (code[i] != 0xFF && code[i] != work[i])
				break;
		}
		if (i == 3) {
			for (i = 0; i < 3; i++)
				code[i] = work[i];
			return true;
		}
	} while (pci_next(bus, dev, fun));
	return false;
}

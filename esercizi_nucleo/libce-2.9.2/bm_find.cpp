#include "libce.h"
#include "bm.h"

using namespace bm;

bool bm_find(natb& bus, natb& dev, natb& fun)
{
	natb code[] = { 0xff, 0x01, 0x01 };

	return pci_find_class(bus, dev, fun, code);
}

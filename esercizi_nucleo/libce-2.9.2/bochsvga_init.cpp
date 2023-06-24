#include "internal.h"
#include "pci.h"
#include "bochsvga.h"

using namespace bochsvga;

// inizializza il descrittore della scheda video "videocard" con i valori necessari
// al funzionamento della bochsvga_config()
bool bochsvga_init()
{
	natb bus = 0, dev = 0, fun = 0;
	natw deviceID = 0x1234;
	natw vendorID = 0x1111;

	//cerchiamo la scheda video basandoci sul deviceID e vendorID
	if(!pci_find_dev(bus, dev, fun, vendorID, deviceID))
	{
		//flog(LOG_WARN, "bochsvga: scheda video non rilevata");
		return false;
	}

	//indirizzo di memoria del framebuffer
	framebuffer = reinterpret_cast<volatile void*>(pci_read_confl(bus, dev, fun, 0x10) & ~0xf);
	//indirizzo dell'area di memoria in cui sono mappati i registri, 1:1, del controller VGA (non usati)
	vgareg = reinterpret_cast<volatile natw*>((pci_read_confl(bus, dev, fun, 0x18) & ~0xf) + 0x0400);
	//indirizzo dell'area di memoria in cui sono mappati i registri, 2:2, delle estensioni BOCHS VBE
	vbeext = reinterpret_cast<volatile natw*>((pci_read_confl(bus, dev, fun, 0x18) & ~0xf) + 0x0500);
	//flog(LOG_INFO, "bochsvga: framebuffer su %p e vbext su %p", videocard.framebuffer, videocard.vbeext);

	return true;
}

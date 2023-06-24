#include "internal.h"
#include "bochsvga.h"

using namespace bochsvga;

// imposta la modalità grafica sulla scheda video, scegliendone la risoluzione
volatile natb* bochsvga_config(natw max_screenx, natw max_screeny)
{
	if(framebuffer == 0)
		return 0;

	//innanzitutto disablitiamo le estensioni BOCHS VBE, come richiesto da specifiche
	vbeext[bochsvga::VBE_DISPI_INDEX_ENABLE] = bochsvga::VBE_DISPI_DISABLED;

	//settiamo risoluzione e profondità di colore
	vbeext[bochsvga::VBE_DISPI_INDEX_XRES] = max_screenx;
	vbeext[bochsvga::VBE_DISPI_INDEX_YRES] = max_screeny;
	vbeext[bochsvga::VBE_DISPI_INDEX_BPP] = bochsvga::VBE_DISPI_BPP_8;

	//chiediamo la modalità di buffer lineare e riattiviamo le estensioni
	vbeext[bochsvga::VBE_DISPI_INDEX_ENABLE] = bochsvga::VBE_DISPI_ENABLED | bochsvga::VBE_DISPI_LFB_ENABLED;

	return static_cast<volatile natb*>(framebuffer);
}

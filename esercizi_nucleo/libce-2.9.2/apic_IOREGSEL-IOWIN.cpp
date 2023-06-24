#include "tipo.h"

namespace apic {

	volatile natl* IOREGSEL = reinterpret_cast<natl*>(0xFEC00000);
	volatile natl* IOWIN = reinterpret_cast<natl*>(0xFEC00010);

}

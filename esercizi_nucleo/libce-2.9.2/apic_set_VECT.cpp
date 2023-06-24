#include "internal.h"
#include "apic.h"

using namespace apic;

void apic_set_VECT(natl ir, natb vec)
{
	// lettura della prima parola lunga dell'entrata
	// ir-esima della tabella interna
        natl work = apic_read_rtl(ir);
	// azzeramento (&) e predisposizione (|) dei bit 7-0 con vec
	work &= 0xFFFFFF00;
	work |= vec;
	// scrittura nella prima parola lunga dell'entrata
	// ir-esima della tabella interna
	apic_write_rtl(ir, work);
}

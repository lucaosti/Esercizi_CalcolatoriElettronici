#include "internal.h"
#include "apic.h"

using namespace apic;

void apic_set_MIRQ(natl ir, bool v)
{
	// lettura della prima parola lunga dell'entrata
	// ir-esima della tabella interna
        natl work = apic_read_rtl(ir);
        if (v)
                work |= 0x00010000;
        else
                work &= 0xFFFEFFFF;
	// scrittura nella prima parola lunga dell'entrata
	// ir-esima della tabella interna
	apic_write_rtl(ir, work);
}

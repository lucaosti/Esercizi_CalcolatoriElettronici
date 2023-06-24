#include "internal.h"
#include "apic.h"

using namespace apic;

void apic_out(natb off, natl v)
{
        *IOREGSEL = off;
        *IOWIN = v;
}

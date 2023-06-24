#include "internal.h"
#include "apic.h"

using namespace apic;

natl apic_in(natb off)
{
        *IOREGSEL = off;
        return *IOWIN;
}

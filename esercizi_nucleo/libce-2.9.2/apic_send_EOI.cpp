#include "internal.h"
#include "apic.h"

using namespace apic;

extern "C" void apic_send_EOI()
{
        *EOIR = 0;
}       

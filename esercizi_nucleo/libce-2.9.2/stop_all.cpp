#include "internal.h"
#include "apic.h"
#include "kbd.h"

extern "C" void stop_all()
{
	cli();
	dtors();
	keyboard_reset();
	clear_screen(0x07);
        apic_reset();
        idt_reset();
        unload_gdt();
}

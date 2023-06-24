#include "internal.h"
#include "apic.h"
#include "kbd.h"
#include "bochsvga.h"
#include "vid.h"
#include "com1.h"
#include "com2.h"

extern "C" bool init_all()
{
        init_gdt();
	load_gdt();
        init_idt(); // chiama gate_init per le prime 32 entrate
	bochsvga_init();
	clear_screen(vid::attr);
	ini_COM1();
	ini_COM2();
        apic_init();
	apic_reset();
	keyboard_init();
	ctors();
	sti();
	return true;
}

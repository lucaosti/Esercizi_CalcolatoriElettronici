#include <libce.h>
#include <apic.h>
#include <vid.h>
#include <kbd.h>

using namespace vid;
using namespace kbd;

volatile bool fine = false;

extern "C" void c_tastiera(){
	natb c = inputb(iRBR);
	if (c == 0x01)	// make code di ESC
		fine = true;
	apic_send_EOI();
}

const natb durata_s = 10;

extern "C" void c_timer()
{

	static natq remaining = durata_s * 1000; //in ms
	
	remaining -= 50;
	if(remaining == 0){
		str_write("Timer scaduto\n");
		remaining = durata_s * 1000;
	}
	apic_send_EOI();
}

extern "C" void a_tastiera();
extern "C" void a_timer();

const natb KBD_VECT = 0x40;
const natb TIM_VECT = 0x50;


int main()
{
	clear_screen(0x0f);


	apic_set_VECT(1, KBD_VECT);
	gate_init(KBD_VECT, a_tastiera);
	apic_set_MIRQ(1, false);
	enable_intr_kbd();


	apic_set_VECT(2, TIM_VECT);
	gate_init(TIM_VECT, a_timer);
	attiva_timer(50000); // periodo di circa 50ms
	apic_set_TRGM(2, false); // false: fronte, true: livello
	apic_set_MIRQ(2, false);

	while (!fine) {
		
	}
	return 0;
}

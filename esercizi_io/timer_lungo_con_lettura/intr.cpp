#include <libce.h>
#include <apic.h>
#include <vid.h>
#include <kbd.h>

using namespace vid;
using namespace kbd;

volatile bool fine = false;
volatile bool attesa = true;
volatile natl numero_letto = 0;

void stampa_intero(natl numero){

	if(numero/10 != 0){
		stampa_intero(numero/10);
	}

	char_write(numero % 10 + '0');

}

extern "C" void c_tastiera(){
	natb c = inputb(iRBR);

	if(c & 0x80){	//ignoro le interruzioni quando sollevo il tasto
		apic_send_EOI();
		return;
	}

	static natl numero = 0;

	if (c == 0x01)	// make code di ESC
		fine = true;

	if(numero_letto){	//non stampo nuovi numeri se ho già letto
		apic_send_EOI();
		return;
	}

	if(c == 0x1C){ //make code di invio
		char_write('\n');
		numero_letto = numero;
		attesa = false;
		apic_send_EOI();
		return;
	}


	natb carattere = conv(c); //converto da make code ad ascii
	char_write(carattere);
	carattere -= '0';	//converto da ascii a numero

	numero *= 10;
	numero += carattere;

	apic_send_EOI();
}


natq remaining;

extern "C" void c_timer(){

	remaining -= 50;
	if(remaining == 0){
		str_write("Fine Timer!\n");
		remaining = numero_letto * 1000;
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

	str_write("Digita un numero\n");

	while(attesa) //attendo che il numero venga letto
		;

	str_write("Numero Letto Correttamente\n");

	stampa_intero(numero_letto);
	char_write('\n');

	remaining = numero_letto * 1000;

	apic_set_VECT(2, TIM_VECT);
	gate_init(TIM_VECT, a_timer);
	attiva_timer(50000); // periodo di circa 50ms
	apic_set_TRGM(2, false); // false: fronte, true: livello
	apic_set_MIRQ(2, false);

	while (!fine) {
	}
	
	return 0;
}

#line 1 "utente/prog/pstack.in"
#include <all.h>
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#define dbg(s, ...) flog(LOG_DEBUG, "TEST %d: " s, test_num, ## __VA_ARGS__)
#define msg(s, ...) printf("TEST %d PROC %d: " s "\n", test_num, getpid(), ## __VA_ARGS__)
#define err(s, ...) msg("ERRORE: " s, ## __VA_ARGS__)
#define die(s, ...) do { err(s, ## __VA_ARGS__); goto error; } while (0)

#define new_proc(tn, pn)\
	t##tn##p##pn = activate_p(t##tn##p##pn##b, test_num, prio--, LIV_UTENTE);

natl end_test; // sync

#define end_subtest() do {\
	(void)&&error;\
error:\
	terminate_p();\
} while (0)

#define end_test() do {\
	(void)&&error;\
error:\
	sem_signal(end_test);\
	terminate_p();\
} while (0)

#define TCNT(n)	natl t##n##m0; natl t##n##n0;
#define testok(n) do {\
	sem_wait(t##n##m0);\
	t##n##n0++;\
	sem_signal(t##n##m0);\
} while (0)

#define checkpf(instr, expected) do {\
	natl _before = getmeminfo().exceptions;\
	instr;\
	natl _after = getmeminfo().exceptions;\
	if (_after - _before != expected) {\
		err("attesi %d page fault, osservati %d", expected,\
			_after - _before);\
		goto error;\
	}\
} while (0)

int test_num;

const char *b2s(bool b)
{
	return b ? "true" : "false";
}

char dummy;

///**********************************************************************
// *             test 00: errori vari                                   *
// *                                                                    *
// * Il primo processo cerca di estendere lo stack oltre il limite. Il  *
// * secondo causa un page fault fuori dall'area utente/privata e il    *
// * terzo causa una eccezione diversa dal page fault. Ci aspettiamo    *
// * tutti e tre vengano abortiti.                                      *
// **********************************************************************/

natl t00p0;
natl t00p1;
natl t00p2;

void t00p0b(natq test)
{
	char buf[2 * DIM_USR_STACK];

	buf[0] = 0;
	err("accesso allo stack oltre il limite non ha causato abort");
	dummy = buf[0];
	terminate_p();
}

void t00p1b(natq test_num)
{
	char *p = (char *)&t00p1b + 0x400000;
	*p = 0;
	err("accesso a utente/convisa non mappato non ha causato abort");
	terminate_p();
}

void t00p2b(natq test_num)
{
	asm("hlt");
	err("eccezione non di page fault non ha causato abort");
	terminate_p();
}

// **********************************************************************
// *             test 01: funzionalità minima                           *
// *                                                                    *
// * Estendiamo lo stack di una sola pagina e accediamo in lettura. Ci  *
// * aspettiamo che funzioni e che venga generato esattamente un page   *
// * fault.                                                             *
// **********************************************************************

natl t01p0;
TCNT(01);

void t01p0b(natq test_num)
{
	natq local;
	char *p = (char *)&local - DIM_PAGINA;
	checkpf(dummy = *p, 1);
	testok(01);
	end_test();
}

// **********************************************************************
// *             test 02: accesso in scrittura                          *
// *                                                                    *
// * Come sopra, ma accediamo in scrittura. Anche questa volta deve     *
// * funzionare, generando esattamente un page fault.                   *
// **********************************************************************

natl t02p0;
TCNT(02);

void t02p0b(natq test_num)
{
	natq local;
	char *p = (char *)&local - DIM_PAGINA;
	checkpf(*p = 0, 1);
	testok(02);
	end_test();
}

// **********************************************************************
// *             test 03: estensione minima                             *
// *                                                                    *
// * Controlliamo che lo stack venga esteso solo del minimo necessario, *
// * come richiesto dal testo.                                          *
// **********************************************************************/

natl t03p0;
TCNT(03);

void t03p0b(natq test_num)
{
	natq local;
	char *p = (char *)&local - DIM_PAGINA;
	checkpf(*p = 0, 1);
	p -= DIM_PAGINA;
	checkpf(*p = 0, 1);
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: estensione completa                           *
// *                                                                    *
// * Controlliamo che l'estensione comprenda tutte le pagine, da quella *
// * che contiene l'indirizzo non tradotto fino alla base dello stack   *
// **********************************************************************/
natl t04p0;
TCNT(04);

void t04p0b(natq test_num)
{
	natq local;
	char *p = (char *)&local - 4*DIM_PAGINA;
	checkpf(dummy = *p, 1);
	for (int i = 0; i < 3; i++) {
		p += DIM_PAGINA;
		checkpf(dummy = *p, 0);
	}
	testok(04);
	end_test();
}

///**********************************************************************
// *             test 05: estensione in due passi                       *
// *                                                                    *
// * Controlliamo che sia ancora possibile estendere lo stack dopo      *
// * averlo esteso gia' una volta.                                      *
// **********************************************************************/
natl t05p0;
TCNT(05);

void t05p0b(natq test_num)
{
	natq local;
	char *p = (char *)&local - 2*DIM_PAGINA;
	checkpf(dummy = *p, 1);
	p -= 2*DIM_PAGINA;
	checkpf(dummy = *p, 1);
	testok(05);
	end_test();
}

///**********************************************************************
// *             test 06: estensione in piu' passi olte i limiti        *
// *                                                                    *
// * Estendiamo lo stack una pagina alla volta, fino a superare il      *
// * limite di DIM_USR_STACK. Ci aspettiamo che il processo venga       *
// * abortito.                                                          *
// **********************************************************************/
natl t06p0;
natl t06p1;
natl t06l0;
TCNT(06);

void t06p0b(natq test_num)
{
	natq local;
	char *p = (char *)&local - DIM_PAGINA;
	for (unsigned int i = 0; i < DIM_USR_STACK/DIM_PAGINA + 1; i++) {
		checkpf(dummy = *p, 1);
		p -= DIM_PAGINA;
	}
	t06l0 = 1;
	end_subtest();
}

void t06p1b(natq test_num)
{
	if (t06l0 == 0)
		testok(06);
	end_test();
}


/**********************************************************************/



#line 232 "utente/prog/pstack.in"
extern natl mainp;
#line 232 "utente/prog/pstack.in"
void main_body(natq id)
#line 233 "utente/prog/pstack.in"
{
	natl prio = 600;
	natl membefore = getmeminfo().num_frame_liberi;

	end_test = sem_ini(0);

	test_num = 0;
	dbg(">>>INIZIO<<<: errori vari");
	new_proc(00, 0);
	delay(10);
	new_proc(00, 1);
	delay(10);
	new_proc(00, 2);
	delay(10);
	dbg("=== FINE ===");

	test_num = 1;
	dbg(">>>INIZIO<<<: funzionalita' minima");
	t01m0 = sem_ini(1);
	new_proc(01, 0);
	sem_wait(end_test);
	if (t01n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 2;
	dbg(">>>INIZIO<<<: accesso in scrittura");
	t02m0 = sem_ini(1);
	new_proc(02, 0);
	sem_wait(end_test);
	if (t02n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>INIZIO<<<: estensione minima");
	t03m0 = sem_ini(1);
	new_proc(03, 0);
	sem_wait(end_test);
	if (t03n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 4;
	dbg(">>>INIZIO<<<: estensione completa");
	t04m0 = sem_ini(1);
	new_proc(04, 0);
	sem_wait(end_test);
	if (t04n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 5;
	dbg(">>>INIZIO<<<: estensione in piu' passi");
	t05m0 = sem_ini(1);
	new_proc(05, 0);
	sem_wait(end_test);
	if (t05n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 6;
	dbg(">>>INIZIO<<<: estensione in piu' passi olte i limiti");
	t06m0 = sem_ini(1);
	new_proc(06, 0);
	new_proc(06, 1);
	sem_wait(end_test);
	if (t06n0 == 1) msg("OK");
	dbg("=== FINE ===");

	delay(10);
	test_num = 7;
	// controlliamo che tutta la memoria sia stata correttamente rilasciata
	natl memafter = getmeminfo().num_frame_liberi;
	if (memafter != membefore)
		err("memoria non liberata: %d frame", membefore - memafter);

	pause();

	terminate_p();
}
natl mainp;
#line 315 "utente/utente.cpp"

void main()
{
	mainp = activate_p(main_body, 0, 900, LIV_UTENTE);

	terminate_p();}

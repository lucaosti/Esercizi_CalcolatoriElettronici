#line 1 "utente/prog/pma.in"
#include <all.h>
#include <vm.h>
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

const vaddr ini_utn_p = norm(I_UTN_P * dim_region(MAX_LIV - 1)); // utente privata

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


natl tokmux;
#define TCNT(n)	natl t##n##n0;
#define testok(n) do {\
	sem_wait(tokmux);\
	t##n##n0++;\
	sem_signal(tokmux);\
} while (0)

int test_num;

const char *b2s(bool b)
{
	return b ? "true" : "false";
}

template<typename T>
const char *p2s(T b)
{
	return b ? "un puntatore" : "nullptr";
}

#define check_ma_attach(pid_, npag_, exp_) ({ \
	bool b_ = ma_attach(pid_, npag_); \
	if (b_ != (exp_)) { \
		die("ma_attach(%d, %d) ha restituito %s invece di %s", pid_, npag_, b2s(b_), b2s(exp_)); \
	} \
	b_; \
})

#define check_ma_wait(exp_) ({\
	vaddr v_ = ma_wait(); \
	if (v_ != (exp_)) { \
		die("ma_wait() ha restituito %p invece di %p", v_, exp_); \
	} \
	v_; \
})

#define check_ma_map(src_, exp_) ({ \
	bool b_ = ma_map(src_); \
	if (b_ != (exp_)) { \
		die("ma_map(%p) ha restituito %s invece di %s", src_, b2s(b_), b2s(exp_)); \
	} \
	b_; \
})

natl npfmutex;
natl expected_npf;
void add_expected_npf(int n)
{
	sem_wait(npfmutex);
	expected_npf += n;
	sem_signal(npfmutex);
}

natb buf[MAX_MA_PAGES] __attribute__((aligned(DIM_PAGINA)));

///**********************************************************************
// *             test 00: errori vari                                   *
// **********************************************************************/

natl t00p0;
natl t00p1;
natl t00p2;
natl t00p3;
natl t00p4;
natl t00p5;
natl t00p6;
natl t00p7;
natl t00p8;
natl t00p9;
natl t00pA;
natl t00s0;

void t00p0b(natq test_num)
{
	ma_attach(MAX_PROC_ID + 1, DIM_PAGINA);
	err("ma_attach con pid non valido non ha causato abort");
	end_subtest();
}

void t00p1b(natq test_num)
{
	ma_attach(getpid(), DIM_PAGINA);
	err("ma_attach ricorsiva non ha causato abort");
	end_subtest();
}

void t00p2b(natq test_num)
{
	sem_wait(t00s0);
	end_subtest();
}

void t00p3b(natq test_num)
{
	ma_attach(t00p2, 0);
	err("ma_attach con npag == 0 non ha causato abort");
	end_subtest();
}

void t00p4b(natq test_num)
{
	ma_attach(t00p2, MAX_MA_PAGES + 1);
	err("ma_attach con npag troppo grande non ha causato abort");
	end_subtest();
}

void t00p5b(natq test_num)
{
	check_ma_attach(t00p2, 2, true);
	ma_attach(t00p2, 2);
	err("ma_attach ripetuta non causato abort");
	end_subtest();
}

void t00p6b(natq test_num)
{
	ma_wait();
	err("ma_wait senza ma_attach non causato abort");
	end_subtest();
}

void t00p7b(natq test_num)
{
	ma_map(reinterpret_cast<vaddr>(buf));
	err("ma_map senza ma_attach non causato abort");
	end_subtest();
}

void t00p8b(natq test_num)
{
	check_ma_attach(t00p2, 2, true);
	ma_map(reinterpret_cast<vaddr>(t00p8b)& ~(DIM_PAGINA-1));
	err("ma_map con src non accessibile in scrittura non ha causato abort");
	end_subtest();
}

void t00p9b(natq test_num)
{
	check_ma_attach(t00p2, 2, true);
	ma_map(DIM_PAGINA);
	err("ma_map con src non accessibile da livello utente non ha causato abort");
	end_subtest();
}

void t00pAb(natq test_num)
{
	sem_signal(t00s0);
	end_subtest();
}

// **********************************************************************
// *             test 01: funzionalità minima                           *
// **********************************************************************

natl t01p0;
natl t01p1;
natl t01s0;
natb *t01w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(01);

void t01p0b(natq test_num)
{
	sem_wait(t01s0);
	t01w0[10] = 1;	
	testok(01);
	end_test();
}

void t01p1b(natq test_num)
{
	check_ma_attach(t01p0, 1, true);
	sem_signal(t01s0);
	check_ma_wait(ini_utn_p + 10);
	check_ma_map(reinterpret_cast<vaddr>(buf), true);
	testok(01);
	end_test();
}

// **********************************************************************
// *             test 02: ma grandi                                     *
// **********************************************************************

natl t02p0;
natl t02p1;
natl t02s0;
natb *t02w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(02);

void t02p0b(natq test_num)
{
	sem_wait(t02s0);
	t02w0[DIM_PAGINA+40] = 1;	
	t02w0[100] = 2;	
	testok(02);
	end_test();
}

void t02p1b(natq test_num)
{
	check_ma_attach(t02p0, 2, true);
	sem_signal(t02s0);
	check_ma_wait(ini_utn_p + DIM_PAGINA + 40);
	check_ma_map(reinterpret_cast<vaddr>(buf)+DIM_PAGINA, true);
	check_ma_wait(ini_utn_p + 100);
	check_ma_map(reinterpret_cast<vaddr>(buf), true);
	testok(02);
	end_test();
}

// **********************************************************************
// *             test 03: terminazione prematura                        *
// **********************************************************************

natl t03p0;
natl t03p1;
natl t03s0;
natb *t03w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(03);

void t03p0b(natq test_num)
{
	sem_wait(t03s0);
	t03w0[DIM_PAGINA-1] = 3;	
	testok(03);
	end_test();
}

void t03p1b(natq test_num)
{
	check_ma_attach(t03p0, 2, true);
	sem_signal(t03s0);
	check_ma_wait(ini_utn_p + DIM_PAGINA-1);
	check_ma_map(reinterpret_cast<vaddr>(buf), true);
	check_ma_wait(0);
	testok(03);
	end_test();
}

// **********************************************************************
// *             test 04: condivisione                                  *
// **********************************************************************

natl t04p0;
natl t04p1;
natl t04s0;
natb *t04w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(04);

void t04p0b(natq test_num)
{
	sem_wait(t04s0);
	if (t04w0[10] == 4) {
		t04w0[10] = 5;
		testok(04);
	}
	end_test();
}

void t04p1b(natq test_num)
{
	check_ma_attach(t04p0, 1, true);
	buf[10] = 4;
	sem_signal(t04s0);
	check_ma_wait(ini_utn_p + 10);
	check_ma_map(reinterpret_cast<vaddr>(buf), true);
	if (buf[10] == 5) {
		testok(04);
	}
	end_test();
}

// **********************************************************************
// *             test 05: terminazione prematura 2                      *
// **********************************************************************

natl t05p0;
natl t05p1;
natl t05p2;
natl t05s0;
natb *t05w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(05);

void t05p0b(natq test_num)
{
	sem_wait(t05s0);
	t05w0[1000] = 2;	
	end_test();
}

void t05p1b(natq test_num)
{
	check_ma_attach(t05p0, 1, true);
	buf[1000] = 1;	
	testok(05);
	end_test();
}

void t05p2b(natq test_num)
{
	sem_signal(t05s0);
	if (buf[1000] == 1)
		testok(05);
	end_test();
}


#line 340 "utente/prog/pma.in"
extern natl mainp;
#line 340 "utente/prog/pma.in"
void main_body(natq id)
#line 341 "utente/prog/pma.in"
{
	natl prio = 600;
	natl membefore = getmeminfo().num_frame_liberi;

	end_test = sem_ini(0);
	tokmux = sem_ini(1);

	test_num = 0;
	dbg(">>>INIZIO<<<: errori vari");
	t00s0 = sem_ini(0);
	new_proc(00, 0);
	new_proc(00, 1);
	new_proc(00, 2);
	new_proc(00, 3);
	new_proc(00, 4);
	new_proc(00, 5);
	new_proc(00, 6);
	new_proc(00, 7);
	new_proc(00, 8);
	new_proc(00, 9);
	new_proc(00, A);
	delay(10);
	dbg("=== FINE ===");

	test_num = 1;
	dbg(">>>INIZIO<<<: funzionalita' minima");
	t01s0 = sem_ini(0);
	new_proc(01, 0);
	new_proc(01, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t01n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 2;
	dbg(">>>INIZIO<<<: ma grandi");
	t02s0 = sem_ini(0);
	new_proc(02, 0);
	new_proc(02, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t02n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>INIZIO<<<: scrittura del destinatario");
	t03s0 = sem_ini(0);
	new_proc(03, 0);
	new_proc(03, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t03n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 4;
	dbg(">>>INIZIO<<<: memory area grandi");
	t04s0 = sem_ini(0);
	new_proc(04, 0);
	new_proc(04, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t04n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 5;
	dbg(">>>INIZIO<<<: terminazione prematura 2");
	t05s0 = sem_ini(0);
	new_proc(05, 0);
	new_proc(05, 1);
	new_proc(05, 2);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t05n0 == 2) msg("OK");
	dbg("=== FINE ===");

	delay(10);
	test_num = 6;
	// controlliamo che tutta la memoria sia stata correttamente rilasciata
	meminfo m = getmeminfo();
	natl memafter = m.num_frame_liberi;
	if (memafter != membefore)
		err("memoria non liberata: %d frame", membefore - memafter);
	pause();

	terminate_p();
}
natl mainp;
#line 433 "utente/utente.cpp"

void main()
{
	mainp = activate_p(main_body, 0, 900, LIV_UTENTE);

	terminate_p();}

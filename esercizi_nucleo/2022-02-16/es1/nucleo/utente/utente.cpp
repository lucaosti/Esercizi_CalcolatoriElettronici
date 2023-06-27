#line 1 "utente/prog/pma.in"
#include <all.h>
#include <vm.h>
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

const vaddr ini_utn_p = norm(I_UTN_P * dim_region(MAX_LIV - 1)); // inizio utente privata
const vaddr ini_utn_c = norm(I_UTN_C * dim_region(MAX_LIV - 1)); // utente condivisa
const vaddr fin_utn_c = ini_utn_c + dim_region(MAX_LIV - 1) * N_UTN_C;

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

vaddr align(vaddr v)
{
	return v & ~(DIM_PAGINA - 1);
}

const char *b2s(bool b)
{
	return b ? "true" : "false";
}

#define FLAGS_BUF 80
const char *f2s(natq f, char *buf)
{
	if (f == 0xffffffffffffffff)
		return "ffffffffffffffff";

	static const char * flags[] = { "P", "RW", "US", "PWT", "PCD", "A", "D", "PS" };
	int n = snprintf(buf, FLAGS_BUF, "%p [ ", f) - 1;
	for (int i = 7; i >= 0; i--) {
		if (f & (1UL << i)) {
			n += snprintf(buf + n, FLAGS_BUF - n, "%s ", flags[i]) - 1;
		}
	}
	snprintf(buf + n, FLAGS_BUF - n, "]");
	return buf;
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

#define check_ma_wait(exp_v_, exp_w_) ({\
	ma_fault f_ = ma_wait(); \
	if (f_.v != (exp_v_) || f_.is_write != (exp_w_)) { \
		die("ma_wait() ha restituito {%p, %s} invece di {%p, %s}", f_.v, b2s(f_.is_write), exp_v_, b2s(exp_w_)); \
	} \
	f_; \
})

#define check_ma_map(dst_, src_, P_, W_, exp_) ({ \
	natq b_ = ma_map(dst_, align(src_), P_, W_); \
	char flags1[FLAGS_BUF], flags2[FLAGS_BUF]; \
	if (b_ != (exp_)) { \
		die("ma_map(%p, %p, %s, %s) ha restituito %s invece di %s", \
			dst_, align(src_), b2s(P_), b2s(W_), f2s(b_, flags1), f2s(exp_, flags2)); \
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
	sem_wait(t00s0);
	end_subtest();
}

void t00p1b(natq test_num)
{
	ma_map(ini_utn_p, reinterpret_cast<vaddr>(buf), true, true);
	err("ma_map senza ma_attach non causato abort");
	end_subtest();
}

void t00p2b(natq test_num)
{
	check_ma_attach(t00p0, 2, true);
	ma_map(ini_utn_p, reinterpret_cast<vaddr>(buf) + 1, true, true);
	err("ma_map con src non allineato alla pagina non ha casusato abort");
	end_subtest();
}

void t00p3b(natq test_num)
{
	check_ma_attach(t00p0, 2, true);
	ma_map(ini_utn_p, ini_utn_c - DIM_PAGINA, true, true);
	err("ma_map con src non in utente/condivisa non ha causato abort");
	end_subtest();
}

void t00p4b(natq test_num)
{
	check_ma_attach(t00p0, 2, true);
	ma_map(ini_utn_p, fin_utn_c - DIM_PAGINA, false, true);
	err("ma_map con src non accessibile non ha causato abort");
	end_subtest();
}

void t00p5b(natq test_num)
{
	check_ma_attach(t00p0, 2, true);
	ma_map(ini_utn_p, reinterpret_cast<vaddr>(t00p5b)& ~(DIM_PAGINA-1), true, true);
	err("ma_map con src non accessibile in scrittura non ha causato abort");
	end_subtest();
}

void t00p6b(natq test_num)
{
	check_ma_attach(t00p0, 2, true);
	ma_map(ini_utn_p, DIM_PAGINA, true, true);
	err("ma_map con src non accessibile da livello utente non ha causato abort");
	end_subtest();
}

void t00p7b(natq test_num)
{
	check_ma_attach(t00p0, 2, true);
	ma_map(ini_utn_p + 1, reinterpret_cast<vaddr>(buf), true, true);
	err("ma_map con dst non allineato alla pagina non ha casusato abort");
	end_subtest();
}

void t00p8b(natq test_num)
{
	check_ma_attach(t00p0, 2, true);
	ma_map(ini_utn_p - DIM_PAGINA, reinterpret_cast<vaddr>(buf), true, true);
	err("ma_map con dst non appartenente alla ma non ha causato abort");
	end_subtest();
}

void t00p9b(natq test_num)
{
	check_ma_attach(t00p0, 2, true);
	ma_map(ini_utn_p + 3 * DIM_PAGINA, reinterpret_cast<vaddr>(buf), true, true);
	err("ma_map con dst non appartenente alla ma non ha causato abort");
	end_subtest();
}

void t00pAb(natq test_num)
{
	sem_signal(t00s0);
	end_subtest();
}

// **********************************************************************
// *             test 01: funzionalità minima (scrittura)               *
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
	ma_fault f;
	check_ma_attach(t01p0, 1, true);
	sem_signal(t01s0);
	f = check_ma_wait(ini_utn_p + 10, true);
	check_ma_map(reinterpret_cast<vaddr>(buf), f.v, true, true, 0);
	ma_cont();
	testok(01);
	end_test();
}

// **********************************************************************
// *             test 02: funzionalità minima (lettura)                 *
// **********************************************************************

natl t02p0;
natl t02p1;
natl t02s0;
natb *t02w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(02);

void t02p0b(natq test_num)
{
	sem_wait(t02s0);
	natb x = t02w0[20];
	if (x != 2)
		err("letto %d invece di 2", x);
	testok(02);
	end_test();
}

void t02p1b(natq test_num)
{
	ma_fault f;
	check_ma_attach(t02p0, 1, true);
	buf[20] = 2;
	sem_signal(t02s0);
	f = check_ma_wait(ini_utn_p + 20, false);
	check_ma_map(reinterpret_cast<vaddr>(buf), f.v, true, false, 0);
	ma_cont();
	testok(02);
	end_test();
}

// **********************************************************************
// *             test 03: upgrade lettura->scrittura                    *
// **********************************************************************

natl t03p0;
natl t03p1;
natl t03s0;
natb *t03w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(03);

void t03p0b(natq test_num)
{
	sem_wait(t03s0);
	natb x = t03w0[30];
	if (x != 3)
		err("letto %d invece di 2", x);
	t03w0[30] = 4;
	testok(03);
	end_test();
}

void t03p1b(natq test_num)
{
	ma_fault f;
	check_ma_attach(t03p0, 1, true);
	buf[30] = 3;
	sem_signal(t03s0);
	f = check_ma_wait(ini_utn_p + 30, false);
	check_ma_map(reinterpret_cast<vaddr>(buf), f.v, true, false, 0);
	ma_cont();
	f = check_ma_wait(ini_utn_p + 30, true);
	check_ma_map(reinterpret_cast<vaddr>(buf), f.v, true, true, BIT_US|BIT_A|BIT_P);
	ma_cont();
	if (buf[30] != 4)
		err("letto %d invece di 4", t03w0[30]);
	testok(03);
	end_test();
}

// **********************************************************************
// *             test 04: downgrade scrittura->lettura                  *
// **********************************************************************

natl t04p0;
natl t04p1;
natl t04s0;
natb *t04w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(04);

void t04p0b(natq test_num)
{
	sem_wait(t04s0);
	t04w0[40] = 4;
	sem_wait(t04s0);
	int x = t04w0[40];
	if (x != 4)
		err("letto %d invece di 4");
	t04w0[40] = 5;
	testok(04);
	end_test();
}

void t04p1b(natq test_num)
{
	ma_fault f;
	check_ma_attach(t04p0, 1, true);
	check_ma_map(reinterpret_cast<vaddr>(buf), ini_utn_p, true, true, 0);
	sem_signal(t04s0);
	check_ma_map(reinterpret_cast<vaddr>(buf), ini_utn_p, true, false, BIT_US|BIT_A|BIT_D|BIT_RW|BIT_P);
	sem_signal(t04s0);
	f = check_ma_wait(ini_utn_p + 40, true);
	check_ma_map(reinterpret_cast<vaddr>(buf), ini_utn_p, true, true, BIT_US|BIT_A|BIT_P);
	ma_cont();
	testok(04);
	end_test();
}

// **********************************************************************
// *             test 05: unmap                                         *
// **********************************************************************

natl t05p0;
natl t05p1;
natl t05s0;
natb *t05w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(05);

void t05p0b(natq test_num)
{
	sem_wait(t05s0);
	t05w0[40] = 4;
	sem_wait(t05s0);
	int x = t05w0[40];
	if (x != 4)
		err("letto %d invece di 4");
	testok(05);
	end_test();
}

void t05p1b(natq test_num)
{
	ma_fault f;
	check_ma_attach(t05p0, 1, true);
	check_ma_map(reinterpret_cast<vaddr>(buf), ini_utn_p, true, true, 0);
	sem_signal(t05s0);
	check_ma_map(reinterpret_cast<vaddr>(buf), ini_utn_p, false, false, BIT_US|BIT_A|BIT_D|BIT_RW|BIT_P);
	sem_signal(t05s0);
	f = check_ma_wait(ini_utn_p + 40, false);
	check_ma_map(reinterpret_cast<vaddr>(buf), ini_utn_p, true, true, 0);
	ma_cont();
	testok(05);
	end_test();
}

// **********************************************************************
// *             test 06: map multiple                                  *
// **********************************************************************

natl t06p0;
natl t06p1;
natl t06s0;
natb *t06w0 = reinterpret_cast<natb*>(ini_utn_p);
TCNT(06);

void t06p0b(natq test_num)
{
	sem_wait(t06s0);
	t06w0[50 + DIM_PAGINA] = 55;
	t06w0[50] = 5;
	t06w0[50 + DIM_PAGINA] = 65;
	int x = t06w0[50];
	if (x != 65)
		err("letto %d invece di 65", x);
	testok(06);
	end_test();
}

void t06p1b(natq test_num)
{
	ma_fault f;
	check_ma_attach(t06p0, 2, true);
	check_ma_map(reinterpret_cast<vaddr>(buf) + DIM_PAGINA, ini_utn_p + DIM_PAGINA, true, true, 0);
	sem_signal(t06s0);
	f = check_ma_wait(ini_utn_p + 50, true);
	check_ma_map(reinterpret_cast<vaddr>(buf), ini_utn_p, true, true, 0);
	check_ma_map(0, ini_utn_p + DIM_PAGINA, false, false, BIT_US|BIT_A|BIT_D|BIT_RW|BIT_P);
	ma_cont();
	f = check_ma_wait(ini_utn_p + DIM_PAGINA + 50, true);
	check_ma_map(reinterpret_cast<vaddr>(buf), ini_utn_p + DIM_PAGINA, true, true, 0);
	ma_cont();
	testok(06);
	end_test();
}

#if 0
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
	ma_fault f;
	check_ma_attach(t02p0, 2, true);
	sem_signal(t02s0);
	f = check_ma_wait(ini_utn_p + DIM_PAGINA + 40, true);
	check_ma_map(reinterpret_cast<vaddr>(buf)+DIM_PAGINA, align(f.v ), true, true, 0);
	ma_cont();
	f = check_ma_wait(ini_utn_p + 100, true);
	check_ma_map(reinterpret_cast<vaddr>(buf), f.v, true, true, 0);
	ma_cont();
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
	ma_fault f;
	check_ma_attach(t03p0, 2, true);
	sem_signal(t03s0);
	f = check_ma_wait(ini_utn_p + DIM_PAGINA-1, true);
	check_ma_map(reinterpret_cast<vaddr>(buf), f.v, true, true, 0);
	ma_cont();
	check_ma_wait(0, false);
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
	ma_fault f;
	check_ma_attach(t04p0, 1, true);
	buf[10] = 4;
	sem_signal(t04s0);
	f = check_ma_wait(ini_utn_p + 10, false);
	check_ma_map(reinterpret_cast<vaddr>(buf), f.v, true, true, 0);
	ma_cont();
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
#endif


#line 569 "utente/prog/pma.in"
extern natl mainp;
#line 569 "utente/prog/pma.in"
void main_body(natq id)
#line 570 "utente/prog/pma.in"
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
	sem_wait(end_test);
	sem_wait(end_test);
	if (t05n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 6;
	dbg(">>>INIZIO<<<: terminazione prematura 2");
	t06s0 = sem_ini(0);
	new_proc(06, 0);
	new_proc(06, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t06n0 == 2) msg("OK");
	dbg("=== FINE ===");

	delay(10);
	test_num = 7;
	// controlliamo che tutta la memoria sia stata correttamente rilasciata
	meminfo m = getmeminfo();
	natl memafter = m.num_frame_liberi;
	if (memafter != membefore)
		err("memoria non liberata: %d frame", membefore - memafter);
	pause();

	terminate_p();
}
natl mainp;
#line 671 "utente/utente.cpp"

void main()
{
	mainp = activate_p(main_body, 0, 900, LIV_UTENTE);

	terminate_p();}

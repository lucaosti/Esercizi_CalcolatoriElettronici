#line 1 "utente/prog/pbufcache.in"
#include <all.h>

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

int test_num;

///**********************************************************************
// *             test 01: lettura di un blocco                          *
// **********************************************************************/

static const natl first_block = 50;

natl t01p0;
TCNT(01);

natb t01b0[DIM_BLOCK];

void t01p0b(natq test_num)
{
	natl blocks[MAX_BUF_DES];
	int n;
	natl brd, rd, bwr, wr;

	bufcache_status(nullptr, &brd, &bwr);
	readhd_n(t01b0, first_block, 1);
	if ( (n = bufcache_status(blocks, &rd, &wr)) != 1)
		die("la bufcache contiene %d blocchi invece di 1");
	if (blocks[0] != first_block)
		die("la bufcache contiene il blocco %d invece di %d",
			blocks[0], first_block);
	if (rd - brd != 1)
		die("eseguite %d letture invece di una", rd - brd);
	if (wr - bwr != 0)
		die("eseguite %d scritture invece di zero", wr - bwr);
	testok(01);
	end_test();
}

///**********************************************************************
// *             test 02: scrittura di un blocco in cache               *
// **********************************************************************/

natb t02b0[DIM_BLOCK];
natb t02b1[DIM_BLOCK];
natl t02p0;
TCNT(02);

void t02p0b(natq test_num)
{
	natl blocks[MAX_BUF_DES];
	int n;
	natl r1, r2, r3, w1, w2, w3;

	bufcache_status(nullptr, &r1, &w1);
	for (natq j = 0; j < DIM_BLOCK; j++)
		t02b0[j] = 'a';
	writehd_n(t02b0, first_block, 1);
	if ( (n = bufcache_status(blocks, &r2, &w2)) != 1)
		die("la bufcache contiene %d blocchi invece di 1", n);
	if (blocks[0] != first_block)
		die("la bufcache contiene il blocco %d invece di %d",
			blocks[0], first_block);
	if (r2 - r1 != 0)
		die("eseguite %d letture invece di zero", r2 - r1);
	if (w2 - w1 != 0)
		die("eseguite %d scritture invece di zero", w2 - w1);
	readhd_n(t02b1, first_block, 1);
	for (natq j = 0; j < DIM_BLOCK; j++)
		if (t02b1[j] != 'a')
			die("blocco %d, byte %d: letto %c invece di %c",
				first_block, j, t02b1[j], 'a');
	if ( (n = bufcache_status(blocks, &r3, &w3)) != 1)
		die("la bufcache contiene %d blocchi invece di 1", n);
	if (blocks[0] != first_block)
		die("la bufcache contiene il blocco %d invece di %d",
			blocks[0], first_block);
	if (r3 - r2 != 0)
		die("eseguite %d letture invece di zero", r3 - r2);
	if (w3 - w2 != 0)
		die("eseguite %d scritture invece di zero", w3 - w2);
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: sync                                          *
// **********************************************************************/

natl t03p0;
TCNT(03);

natb t03b0[DIM_BLOCK];
natb t03b1[DIM_BLOCK];

void t03p0b(natq test_num)
{
	natl blocks[MAX_BUF_DES];
	int n;
	natl r1, r2, r3, w1, w2, w3;
	natl b = first_block + 3;

	bufcache_status(nullptr, &r1, &w1);
	readhd_n(t03b0, b, 1);
	if ( (n = bufcache_status(blocks, &r2, &w2)) != 2)
		die("la bufcache contiene %d blocchi invece di 2");
	if (r2 - r1 != 1)
		die("eseguite %d letture invece di una", r2 - r1);
	if (w2 - w1 != 0)
		die("eseguite %d scritture invece di zero", w2 - w1);
	synchd();
	if ( (n = bufcache_status(blocks, &r3, &w3)) != 2)
		die("la bufcache contiene %d blocchi invece di 2");
	if (r3 - r2 != 0)
		die("eseguite %d letture invece di zero", r3 - r2);
	if (w3 - w2 != 1)
		die("eseguite %d scritture invece di una", w3 - w2);
	dmareadhd_n(t03b1, first_block, 1);
	for (natq j = 0; j < DIM_BLOCK; j++)
		if (t03b1[j] != 'a')
			die("blocco %d, byte %d: letto %c invece di %c",
				first_block, j, t03b1[j], 'a');
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: scrittura di un blocco non cache              *
// **********************************************************************/

natb t04b0[DIM_BLOCK];
natb t04b1[DIM_BLOCK];
natl t04p0;
TCNT(04);

void t04p0b(natq test_num)
{
	natl blocks[MAX_BUF_DES];
	int n;
	natl r1, r2, r3, w1, w2, w3;
	natl b = first_block + 4;

	bufcache_status(nullptr, &r1, &w1);
	for (natq j = 0; j < DIM_BLOCK; j++)
		t04b0[j] = 'c';
	writehd_n(t04b0, b, 1);
	if ( (n = bufcache_status(blocks, &r2, &w2)) != 3)
		die("la bufcache contiene %d blocchi invece di 3", n);
	for (int i = 0; i < n; i++) {
		if (blocks[i] == b)
			goto found;
	}
	die("la bufcache non contiene il blocco %d", b);
found:
	if (r2 - r1 != 0)
		die("eseguite %d letture invece di zero", r2 - r1);
	if (w2 - w1 != 0)
		die("eseguite %d scritture invece di zero", w2 - w1);
	readhd_n(t04b1, b, 1);
	for (natq j = 0; j < DIM_BLOCK; j++)
		if (t04b1[j] != 'c')
			die("blocco %d, byte %d: letto %c invece di %c",
				b, j, t04b1[j], 'c');
	if ( (n = bufcache_status(blocks, &r3, &w3)) != 3)
		die("la bufcache contiene %d blocchi invece di 3", n);
	if (r3 - r2 != 0)
		die("eseguite %d letture invece di zero", r3 - r2);
	if (w3 - w2 != 0)
		die("eseguite %d scritture invece di zero", w3 - w2);
	testok(04);
	end_test();
}


///**********************************************************************
// *             test 05: rimpiazzamento                                *
// **********************************************************************/

natb t05b0[DIM_BLOCK * MAX_BUF_DES];
natb t05b1[DIM_BLOCK];
natb t05b2[DIM_BLOCK];
natl t05p0;
TCNT(05);

void t05p0b(natq test_num)
{
	natl blocks[MAX_BUF_DES];
	int n;
	natl r1, r2, w1, w2;
	natl b = first_block + 5;
	natl wb = first_block + 4;
	natl m1, m2;

	bufcache_status(nullptr, &r1, &w1);
	readhd_n(t05b0, b, MAX_BUF_DES);
	if ( (n = bufcache_status(blocks, &r2, &w2)) != MAX_BUF_DES)
		die("la bufcache contiene %d blocchi invece di %d", n, MAX_BUF_DES);
	if (r2 - r1 != MAX_BUF_DES)
		die("eseguite %d letture invece di %d", r2 - r1, MAX_BUF_DES);
	if (w2 - w1 != 1)
		die("eseguite %d scritture invece di 1", w2 - w1);
	m1 = 0;
	for (int i = 0; i < MAX_BUF_DES; i++)
		m1 |= 1U << (5 + i);
	m2 = 0;
	for (int i = 0; i < MAX_BUF_DES; i++)
		m2 |= 1U << (blocks[i] - first_block);
	if (m1 != m2)
		die("la bufcache non contiene tutti e soli i blocchi attesi");
	dmareadhd_n(t05b2, wb, 1);
	for (natq j = 0; j < DIM_BLOCK; j++)
		if (t05b2[j] != 'c')
			die("blocco %d, byte %d: letto %c invece di %c",
				wb, j, t05b2[j], 'c');
	testok(05);
	end_test();
}

///**********************************************************************
// *             test 06: sync 2                                        *
// **********************************************************************/

natl t06p0;
TCNT(06);

natb t06b0[DIM_BLOCK * 2];
natb t06b1[DIM_BLOCK * 2];

void t06p0b(natq test_num)
{
	natl blocks[MAX_BUF_DES];
	int n;
	natl r1, r2, r3, r4, w1, w2, w3, w4;
	natl b = first_block + 6;

	bufcache_status(nullptr, &r1, &w1);
	for (natq j = 0; j < DIM_BLOCK * 2; j++)
		t06b0[j] = 'd';
	writehd_n(t06b0, b, 2);
	if ( (n = bufcache_status(blocks, &r2, &w2)) != MAX_BUF_DES)
		die("la bufcache contiene %d blocchi invece di %d", MAX_BUF_DES);
	if (r2 - r1 != 0)
		die("eseguite %d letture invece di zero", r2 - r1);
	if (w2 - w1 != 0)
		die("eseguite %d scritture invece di zero", w2 - w1);
	synchd();
	if ( (n = bufcache_status(blocks, &r3, &w3)) != MAX_BUF_DES)
		die("la bufcache contiene %d blocchi invece di %d", MAX_BUF_DES);
	if (r3 - r2 != 0)
		die("eseguite %d letture invece di zero", r3 - r2);
	if (w3 - w2 != 2)
		die("eseguite %d scritture invece di 2", w3 - w2);
	dmareadhd_n(t06b1, b, 2);
	for (natq j = 0; j < DIM_BLOCK; j++)
		if (t06b1[j] != 'd')
			die("blocco %d, byte %d: letto %c invece di %c",
				b, j, t06b1[j], 'd');
	synchd();
	if ( (n = bufcache_status(blocks, &r4, &w4)) != MAX_BUF_DES)
		die("la bufcache contiene %d blocchi invece di %d", n, MAX_BUF_DES);
	if (r4 - r3 != 0)
		die("eseguite %d letture invece di zero", r4 - r3);
	if (w4 - w3 != 0)
		die("eseguite %d scritture invece di zero", w4 - w3);
	testok(06);
	end_test();
}
/**********************************************************************/



#line 300 "utente/prog/pbufcache.in"
extern natl mainp;
#line 300 "utente/prog/pbufcache.in"
void main_body(natq id)
#line 301 "utente/prog/pbufcache.in"
{
	natl prio = 600;

	end_test = sem_ini(0);

	test_num = 1;
	dbg(">>>INIZIO<<<: lettura di un blocco");
	t01m0 = sem_ini(1);
	new_proc(01, 0);
	sem_wait(end_test);
	if (t01n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 2;
	dbg(">>>INIZIO<<<: scrittura di un blocco in cache");
	t02m0 = sem_ini(1);
	new_proc(02, 0);
	sem_wait(end_test);
	if (t02n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>INIZIO<<<: sync");
	t03m0 = sem_ini(1);
	new_proc(03, 0);
	sem_wait(end_test);
	if (t03n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 4;
	dbg(">>>INIZIO<<<: lettura da cache");
	t04m0 = sem_ini(1);
	new_proc(04, 0);
	sem_wait(end_test);
	if (t04n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 5;
	dbg(">>>INIZIO<<<: riempimento cache");
	t05m0 = sem_ini(1);
	new_proc(05, 0);
	sem_wait(end_test);
	if (t05n0 == 1) msg("OK");
	dbg("=== FINE ===");


	test_num = 6;
	dbg(">>>INIZIO<<<: rimpiazzamento");
	t06m0 = sem_ini(1);
	new_proc(06, 0);
	sem_wait(end_test);
	if (t06n0 == 1) msg("OK");
	dbg("=== FINE ===");

	pause();

	terminate_p();
}
natl mainp;
#line 365 "utente/utente.cpp"

void main()
{
	mainp = activate_p(main_body, 0, 900, LIV_UTENTE);

	terminate_p();}

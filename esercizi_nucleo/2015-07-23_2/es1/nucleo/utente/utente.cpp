#line 1 "utente/prog/pshmem.in"
#include <all.h>


#line 4 "utente/prog/pshmem.in"
extern natl init;
#line 4 "utente/prog/pshmem.in"
extern natl sync;
#line 6 "utente/prog/pshmem.in"
const int NSHMEM = 2;
const natl shmem_size[NSHMEM] = { 500, 500 };
natl shmem[NSHMEM];
void shw(natq a)
{
	natl *shm[2] = {
		static_cast<natl*>(shmem_attach(shmem[0])),
		static_cast<natl*>(shmem_attach(shmem[1]))
	};

	shmem_detach(shmem[1]);
	printf("proc%d: detached shmem %d\n", a, shmem[1]);
	for (int i = 0; i < NSHMEM; i++) {
		natl *ptr = shm[i];
		printf("proc%d: begin writing shmem %d\n", a, shmem[i]);
		for (natl j = 0; j < shmem_size[i]; j++) {
			*ptr = j;
			ptr += 1024;
		}
		printf("proc%d: end writing shmem %d\n", a, shmem[i]);
	}
	terminate_p();
}

void shr(natq a)
{
	natl *shm[2] = {
		static_cast<natl*>(shmem_attach(shmem[0])),
		static_cast<natl*>(shmem_attach(shmem[1]))
	};

	for (int i = 0; i < NSHMEM; i++) {
		natl *ptr = shm[i];
		natl sum = 0;
		printf("proc%d: begin reading shmem %d\n", a, shmem[i]);
		for (natl j = 0; j < shmem_size[i]; j++) {
			sum += *ptr;
			ptr += 1024;
		}
		printf("proc%d: end reading shmem %d: %d\n", a, shmem[i], sum);
		shmem_detach(shmem[i]);
	}
	terminate_p();
}

void last(natq a)
{
	sem_signal(sync);
	sem_signal(sync);
	terminate_p();
}

void init_body(natq a)
#line 59 "utente/prog/pshmem.in"
{
	for (int i = 0; i < NSHMEM; i++)
		shmem[i] = shmem_create(shmem_size[i]);

	activate_p(last, 3, 10, LIV_UTENTE);
	sem_wait(sync);
	meminfo m1 = getmeminfo();
	activate_p(shw, 0, 50, LIV_UTENTE);
	activate_p(shr, 1, 20, LIV_UTENTE);
	sem_wait(sync);
	meminfo m2 = getmeminfo();
	int tmp;
	if ( (tmp = m1.heap_libero - m2.heap_libero) )
		printf("%d byte non deallocati\n", tmp);
	if ( (tmp = m1.num_frame_liberi - m2.num_frame_liberi) )
		printf("%d frame non deallocati\n", tmp);
	pause();

	terminate_p();
}
natl init;
natl sync;
#line 87 "utente/utente.cpp"

void main()
{
	init = activate_p(init_body, 0, 60, LIV_UTENTE);
	sync = sem_ini(0);

	terminate_p();}

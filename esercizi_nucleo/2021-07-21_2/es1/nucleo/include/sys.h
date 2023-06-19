extern "C" natl activate_p(void f(natq), natq a, natl prio, natl liv);
extern "C" void terminate_p();
extern "C" natl sem_ini(int val);
extern "C" void sem_wait(natl sem);
extern "C" void sem_signal(natl sem);
extern "C" void delay(natl n);
// ( ESAME 2021-07-21

struct des_ma {
	void* base;
	natq  npag;
};

extern "C" void*   macreate(natq size);
extern "C" bool    masend(void *v, natl pid);
extern "C" des_ma  marecv();
//   ESAME 2021-07-21 )

extern "C" void do_log(log_sev sev, const char* buf, natl quanti);

// informazioni a puro scopo di debug/correzione compiti
struct meminfo {
	natl heap_libero;
	natl num_frame_liberi;
	natl pid;
};

extern "C" meminfo getmeminfo();


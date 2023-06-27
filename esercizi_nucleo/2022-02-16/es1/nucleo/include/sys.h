extern "C" natl activate_p(void f(natq), natq a, natl prio, natl liv);
extern "C" void terminate_p();
extern "C" natl sem_ini(int val);
extern "C" void sem_wait(natl sem);
extern "C" void sem_signal(natl sem);
extern "C" void delay(natl n);

extern "C" void do_log(log_sev sev, const char* buf, natl quanti);

// informazioni a puro scopo di debug/correzione compiti
struct meminfo {
	natl heap_libero;
	natl num_frame_liberi;
	natl pid;
};

extern "C" meminfo getmeminfo();

// ( ESAME 2022-02-16
extern "C" bool ma_attach(natw pid, natq npag);

struct ma_fault {
	vaddr v;
	bool  is_write;
};

extern "C" ma_fault ma_wait();
extern "C" natq ma_map(vaddr src, vaddr dst, bool P, bool W);
extern "C" void ma_cont();
//   ESAME 2022-02-16 )

#include "tipo.h"
// Funzioni per leggere da o scrivere in un registro dello spazio di I/O
extern "C" natb inputb(ioaddr reg);
extern "C" void outputb(natb a, ioaddr reg);
extern "C" natw inputw(ioaddr reg);
extern "C" void outputw(natw a, ioaddr reg);
extern "C" natl inputl(ioaddr reg);
extern "C" void outputl(natl a, ioaddr reg);
extern "C" void outputbw(natw vetto[], int quanti, ioaddr reg);
extern "C" void inputbw(ioaddr reg, natw vetti[], int quanti);
// Funzioni per leggere da tastiera
natb get_code();
char conv(natb c);
char char_read();
char char_read_int();
natb get_code_int();
void enable_intr_kbd();
void disable_intr_kbd();
void drain_kbd();
// Funzione per configurare la modalità video
volatile natb* bochsvga_config(natw max_screenx, natw max_screeny);
// Funzioni per scrivere sul video
void clear_screen(natb col);
void char_write(natb c);
void str_write(const char str[]);
// Funzioni per leggere da o scrivere sul primo hard disk
enum hd_cmd {
	WRITE_SECT = 0x30,
	READ_SECT  = 0x20,
	WRITE_DMA  = 0xCA,
	READ_DMA   = 0xC8
};
void hd_start_cmd(natl lba, natb quanti, natb cmd);
void hd_enable_intr();
void hd_disable_intr();
void hd_output_sect(natb *buf);
void hd_input_sect(natb *buf);
void hd_ack_intr();
// funzioni per il bus mastering IDE
bool bm_find(natb& bus, natb& dev, natb& fun);
void bm_init(natb bus, natb dev, natb fun);
void bm_prepare(paddr prd, bool write);
void bm_start();
void bm_ack();
// Funzioni per la porta seriale
void serial_o(natb c);
void serial2_o(natb c);
// Funzioni sulle stringhe
void *memset(void *dest, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char str[]);
char* copy(const char src[], char dst[]);
// timer
void attiva_timer(natw N);
// Funzione di attesa
void pause();
// Funzioni relative al meccanismo di interruzione
extern "C" void gate_init(natl num, void routine());
extern "C" void trap_init(natl num, void routine());
extern "C" void apic_set_MIRQ(natl irq, bool enable);
extern "C" void apic_set_TRGM(natl irq, bool enable);
extern "C" void apic_set_VECT(natl irq, natb vec);
// Funzioni per il bus PCI
natb pci_read_confb(natb bus, natb dev, natb fun, natb regn);
natw pci_read_confw(natb bus, natb dev, natb fun, natb regn);
natl pci_read_confl(natb bus, natb dev, natb fun, natb regn);
void pci_write_confb(natb bus, natb dev, natb fun, natb regn, natb data);
void pci_write_confw(natb bus, natb dev, natb fun, natb regn, natw data);
void pci_write_confl(natb bus, natb dev, natb fun, natb regn, natl data);
bool pci_find_dev(natb& bus, natb& dev, natb& fun, natw vendorID, natw deviceID);
bool pci_find_class(natb& bus, natb& dev, natb& fun, natb code[]);
bool pci_next(natb& bus, natb& dev,natb& fun);
// Funzioni per lo heap
template<typename T>
T constexpr allinea(T v, size_t a) {
	size_t v_ = reinterpret_cast<size_t>(v);
	v_ = (v_ % a == 0 ? v_ : ((v_ + a - 1) / a) * a);
	return reinterpret_cast<T>(v_);
}
void heap_init(void *start, size_t size);
void* alloca(size_t dim);
enum class align_val_t : size_t {};
void* alloc_aligned(size_t dim, align_val_t align);
void dealloca(void *p);
size_t disponibile();

// Funzioni per il log
enum log_sev { LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERR, LOG_USR };
extern "C" void flog(log_sev sev, const char* fmt, ...);
extern "C" void do_log(log_sev sev, const char* buf, natl quanti);

// Funzioni avanzate
#include <stdarg.h>
int printf(const char *fmt, ...);
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap);
int snprintf(char *buf, natl n, const char *fmt, ...);
int int_read();
bool apic_init();
void apic_reset();
extern "C" [[noreturn]] void reboot();
[[noreturn]] void fpanic(const char *fmt, ...);

template<typename T>
T max(T a, T b)
{
	return a < b ? b : a;
}

template<typename T>
T min(T a, T b)
{
	return a < b ? a : b;
}

// allocazione della memoria (overloading di default normalmente forniti
// dalla dalla libreria standard del C++). Si limitano a richiamare in
// modo appropriato 'operator new' e 'operator delete', che devono
// esssere definiti a parte.
void *operator new[](size_t s);
void *operator new[](size_t s, align_val_t a);
void operator delete(void *p, size_t s);
void operator delete[](void *p);
void operator delete[](void *p, size_t s);

// elf
bool find_eh_frame(vaddr elf, vaddr& eh_frame, natq& eh_frame_len);

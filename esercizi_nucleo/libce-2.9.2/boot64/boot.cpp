// sistema.cpp
//
#ifdef __x86_64__
#error "this must be compiled for 32 bit"
#endif
#include "mboot.h"
#define MAX_LOG LOG_ERR
#include "libce.h"
#include "elf64.h"
#include "vm.h"

// nop, ma richieste comunque
natq dummy;
void *operator new(size_t s)
{
	return &dummy;
}

void operator delete(void *p)
{
}

// attiva la paginazione
extern "C" void attiva_paginazione(multiboot_info_t *mbi, natl entry, int max_liv);

void ini_COM1();

natl carica_modulo(multiboot_module_t* mod) {
	Elf64_Ehdr* elf_h = (Elf64_Ehdr*)mod->mod_start;

	if (!(elf_h->e_ident[EI_MAG0] == ELFMAG0 &&
	      elf_h->e_ident[EI_MAG1] == ELFMAG1 &&
	      elf_h->e_ident[EI_MAG2] == ELFMAG2 &&
	      elf_h->e_ident[EI_MAG2] == ELFMAG2))
	{
		flog(LOG_ERR, "Formato del modulo '%s' non riconosciuto",
			mod->cmdline);
		return 0;
	}

	if (!(elf_h->e_ident[EI_CLASS] == ELFCLASS64  &&
	      elf_h->e_ident[EI_DATA]  == ELFDATA2LSB &&
	      elf_h->e_type	       == ET_EXEC     &&
	      elf_h->e_machine 	       == EM_AMD64))
	{ 
		flog(LOG_ERR, "Il modulo '%s' non contiene "
			"un esegubile per x86_64", mod->cmdline);
		return 0;
	}

	Elf64_Phdr* elf_ph = (Elf64_Phdr*)(mod->mod_start + elf_h->e_phoff);
	for (int i = 0; i < elf_h->e_phnum; i++) {
		if (elf_ph->p_type != PT_LOAD)
			continue;

		memcpy((void*)elf_ph->p_vaddr,
		       (void*)(mod->mod_start + elf_ph->p_offset),
		       elf_ph->p_filesz);
		flog(LOG_INFO, "Copiato segmento di %d byte all'indirizzo %p",
			(long)elf_ph->p_filesz, (void*)elf_ph->p_vaddr);
		if (elf_ph->p_memsz != elf_ph->p_filesz) {
			memset((void*)(elf_ph->p_vaddr + elf_ph->p_filesz), 0,
			       elf_ph->p_memsz - elf_ph->p_filesz);
			flog(LOG_INFO, "... azzerati ulteriori %d byte",
					elf_ph->p_memsz - elf_ph->p_filesz);
		}
		elf_ph = (Elf64_Phdr*)((unsigned int)elf_ph +
			elf_h->e_phentsize);
	}
	flog(LOG_INFO, "entry point %p", elf_h->e_entry);
	return (natl)elf_h->e_entry;
}

static const int MAXARGS = 10;
static int debug_mode = 0;

static void
parse_args(char *cmd)
{
	char *argv[MAXARGS];
	int argc = 0;

	char *scan = cmd;
	do {
		argv[argc] = scan;
		while (*scan && *scan != ' ')
			scan++;
		if (*scan) {
			*scan = '\0';
			for (scan++; *scan && *scan == ' '; scan++)
				;
		}
		flog(LOG_INFO, "argv[%d] = '%s'", argc, argv[argc]);
		argc++;
	} while (*scan && argc <= MAXARGS);

	for (int i = 0; i < argc; i++) {
		if (argv[i][0] != '-')
			continue;
		if (argv[i][1] == 's' && !debug_mode) {
			flog(LOG_INFO, "DEBUG MODE");
			debug_mode = 1;
		}
	}
}

extern "C" void panic(const char *)
{
}

#define KiB			1024ULL
#define MiB			(1024ULL*KiB)
#define GiB			(1024ULL*MiB)
#define DIM_PAGINA		4096ULL

int wait_for_gdb;
extern "C" void cmain (natl magic, multiboot_info_t* mbi)
{
	natl entry;
	
	// (* inizializzazione Seriale per il Debugging
	ini_COM1();
	flog(LOG_INFO, "Boot loader Calcolatori Elettronici, v0.02");
	// *)
	
	// (* controlliamo di essere stati caricati
	//    da un bootloader che rispetti lo standard multiboot
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		flog(LOG_ERR, "Numero magico non valido: 0x%x", magic);
	}
	// *)

	if (mbi->flags & MULTIBOOT_INFO_CMDLINE) {
		flog(LOG_INFO, "argomenti: %s", mbi->cmdline);
		parse_args((char *)mbi->cmdline);
	}

	// (* Carichiamo i vari moduli
	//    Il numero dei moduli viene passato dal bootloader in mods_count
	if (!(mbi->flags & MULTIBOOT_INFO_MODS) ||
	      mbi->mods_count < 1)
	{
		flog(LOG_ERR, "Informazioni sui moduli assenti o errate");
		return;
	}

	flog(LOG_INFO, "mods_count = %d, mods_addr = 0x%x",
			mbi->mods_count, mbi->mods_addr);
	multiboot_module_t* mod = (multiboot_module_t*) mbi->mods_addr;
	flog(LOG_INFO, "mod[0]:%s: start 0x%x end 0x%x",
		mod->cmdline, mod->mod_start, mod->mod_end);
	entry = carica_modulo(mod);
	// *)
	//
	
	paddr root_tab = 0x1000;
	paddr tab = root_tab + DIM_PAGINA;
	for (tab_iter it(root_tab, 0, 4*GiB); it; it.next()) {
		tab_entry& e = it.get_e();

		if (it.get_l() > 2) {
			memset((void *)tab, 0, DIM_PAGINA);
			set_IND_FISICO(e, tab);
			e |= BIT_P | BIT_RW | BIT_US;
			tab += DIM_PAGINA;
		} else {
			vaddr v = it.get_v();
			set_IND_FISICO(e, v);
			e |= BIT_P | BIT_RW | BIT_US | BIT_PS;
			if (v < 2*MiB) {
				e |= BIT_PWT;
			} else if (v >= 1*GiB) {
				e |= BIT_PWT | BIT_PCD;
			}
		}
	}
	
	loadCR3(root_tab);
	if (debug_mode) {
		wait_for_gdb = 1;
		flog(LOG_INFO, "Attendo collegamento da gdb...");
	}
	attiva_paginazione(mbi, entry, MAX_LIV);
	
	/* mai raggiunto */
	return;
}
// )

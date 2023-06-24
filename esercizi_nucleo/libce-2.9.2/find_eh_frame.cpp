#include "internal.h"
#include "elf64.h"

static const Elf64_Word SHT_X86_64_UNWIND = 0x70000001;

bool find_eh_frame(vaddr elf, vaddr& eh_frame, natq& eh_frame_len)
{
	Elf64_Ehdr *h = reinterpret_cast<Elf64_Ehdr*>(elf);

	for (Elf64_Half i = 0; i < h->e_shnum; i++) {
		Elf64_Shdr *sh = reinterpret_cast<Elf64_Shdr*>(elf + h->e_shoff + h->e_shentsize * i);
		if (sh->sh_type == SHT_X86_64_UNWIND) {
			eh_frame = sh->sh_addr;
			eh_frame_len = sh->sh_size;
			return true;
		}
	}
	return false;
}

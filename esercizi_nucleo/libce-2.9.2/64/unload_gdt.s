#include "def.h"

.global unload_gdt
unload_gdt:
	#popl %ecx
        ret

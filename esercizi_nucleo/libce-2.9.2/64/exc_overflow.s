#include "def.h"
.global exc_overflow
exc_overflow:
	.cfi_startproc
	.cfi_def_cfa_offset 40
	.cfi_offset rip, -40
	.cfi_offset rsp, -16
	movq $4, %rdi
	movq $0, %rsi
	mov (%rsp), %rdx
	jmp comm_exc
	.cfi_endproc

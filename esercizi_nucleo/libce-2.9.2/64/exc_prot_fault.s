#include "def.h"
.global exc_prot_fault
exc_prot_fault:
	.cfi_startproc
	.cfi_def_cfa_offset 48
	.cfi_offset rip, -40
	.cfi_offset rsp, -16
	movq $13, %rdi
	popq %rsi
	.cfi_adjust_cfa_offset -8
	mov (%rsp), %rdx
	jmp comm_exc
	.cfi_endproc

#include "def.h"
.global exc_invalid_tss
exc_invalid_tss:
	.cfi_startproc
	.cfi_def_cfa_offset 48
	.cfi_offset rip, -40
	.cfi_offset rsp, -16
	movq $10, %rdi
	popq %rsi
	.cfi_adjust_cfa_offset -8
	mov (%rsp), %rdx
	jmp comm_exc
	.cfi_endproc

#include "def.h"
.global exc_int_tipo_pf
exc_int_tipo_pf:
	.cfi_startproc
	.cfi_def_cfa_offset 48
	.cfi_offset rip, -40
	.cfi_offset rsp, -16
	movq $14, %rdi
	popq %rsi
	.cfi_adjust_cfa_offset -8
	mov (%rsp), %rdx
	jmp comm_exc
	.cfi_endproc

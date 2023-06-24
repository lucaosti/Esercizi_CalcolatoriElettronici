exc_simd:
.global exc_simd
	.cfi_startproc
	.cfi_def_cfa_offset 40
	.cfi_offset rip, -40
	.cfi_offset rsp, -16
	movq $19, %rdi
	movq $0, %rsi
	mov (%rsp), %rdx
	jmp comm_exc
	.cfi_endproc

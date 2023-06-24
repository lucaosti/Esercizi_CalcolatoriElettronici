// legge una sequenza di parole da una porta di I/O
	.global inputbw
inputbw:
	.cfi_startproc
	movq %rdx, %rcx
	movw %di, %dx
	movq %rsi, %rdi
	cld
	rep
	insw
	ret
	.cfi_endproc

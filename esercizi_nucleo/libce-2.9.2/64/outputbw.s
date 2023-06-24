// scrive una sequenza di parole in una porta di I/O
	.global outputbw
outputbw:
	.cfi_startproc
	movq %rsi, %rcx
	movq %rdi, %rsi
	cld
	rep
	outsw
	ret
	.cfi_endproc

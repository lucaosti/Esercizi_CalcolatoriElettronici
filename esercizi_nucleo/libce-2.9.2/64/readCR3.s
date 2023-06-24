// restituisce in %eax il contenuto di cr3
	.global readCR3
readCR3:
	.cfi_startproc
	movq %cr3, %rax
	retq
	.cfi_endproc


// restituisce in %eax il contenuto di cr2
.global readCR2
readCR2:
	.cfi_startproc
	movq %cr2, %rax
	retq
	.cfi_endproc


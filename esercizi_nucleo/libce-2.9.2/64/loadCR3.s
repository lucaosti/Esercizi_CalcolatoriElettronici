// carica il registro cr3
// parametri: indirizzo fisico del nuovo direttorio
	.global loadCR3
loadCR3:
	.cfi_startproc
	movq %rdi, %cr3
	retq
	.cfi_endproc


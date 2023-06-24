// dato un indirizzo virtuale (come parametro) usa l'istruzione invlpg per
// eliminare la corrispondente traduzione dal TLB
	.global invalida_entrata_TLB //
invalida_entrata_TLB:
	.cfi_startproc
	invlpg (%rdi)
	ret
	.cfi_endproc


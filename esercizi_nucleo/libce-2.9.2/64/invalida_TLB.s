// invalida tutto il TLB
	.global invalida_TLB
invalida_TLB:
	.cfi_startproc
	// e sufficiente riscrivere in cr3 il suo contenuto corrente
	movq %cr3, %rax
	movq %rax, %cr3
	retq
	.cfi_endproc

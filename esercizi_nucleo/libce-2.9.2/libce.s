.macro salva_registri

	pushq %rax
	pushq %rcx
	pushq %rdx
	pushq %rbx
	pushq %rsi
	pushq %rdi
	pushq %rbp
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

.endm

.macro carica_registri

	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rbp
	popq %rdi
	popq %rsi
	popq %rbx
	popq %rdx
	popq %rcx
	popq %rax

.endm



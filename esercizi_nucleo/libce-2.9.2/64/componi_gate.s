#include "def.h"

.global componi_gate
componi_gate:
	.cfi_startproc
	movq %rsi, %rax		// offset della routine

	movw %ax, (%rdi)  	// primi 16 bit dell'offset
	movw $SEL_CODICE_SISTEMA, 2(%rdi)

	movw $0, %ax
	movb $0b11101110, %ah 	        // byte di accesso
					// (presente, 32bit, tipo interrupt)
	andq $0x1, %rdx
	orb %dl, %ah
	movb $0, %al			// la parte bassa deve essere 0
	movl %eax, 4(%rdi)	// 16 bit piu' sign. dell'offset
					// e byte di accesso
	shrq $32, %rax			//estensione a 64 bit dell'offset
	movl %eax, 8(%rdi)
	movl $0, 12(%rdi) 		//riservato

	ret
	.cfi_endproc

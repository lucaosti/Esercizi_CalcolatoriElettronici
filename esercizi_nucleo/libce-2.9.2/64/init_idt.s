#include "def.h"
// carica un gate nella IDT
// parametri: 
// num: indice (a partire da 0) in IDT del gate da caricare
// routine: indirizzo della routine da associare al gate
.macro carica_gate num routine

	movabsq $(idt + \num*16), %rdi
	movq $\routine, %rsi
	call componi_gate

.endm


.global init_idt
init_idt:
	.cfi_startproc
        pushq %rcx
	.cfi_adjust_cfa_offset 8
        pushq %rax
	.cfi_adjust_cfa_offset 8
        pushq %rdi
	.cfi_adjust_cfa_offset 8

        movabsq $idt, %rdi
        movq $512, %rcx
        movq $0, %rax
        cld
        rep stosq

	carica_gate	0 		exc_divide_error 	
	carica_gate	1 		exc_debug 		
	carica_gate	2 		exc_nmi 		
	carica_gate	3 		exc_breakpoint 	
	carica_gate	4 		exc_overflow 	
	carica_gate	5 		exc_bound_re 	
	carica_gate	6 		exc_invalid_opcode	
	carica_gate	7 		exc_dev_na 		
	carica_gate	8 		exc_double_fault 	
	carica_gate	9 		exc_coproc_so 	
	carica_gate	10 		exc_invalid_tss 	
	carica_gate	11 		exc_segm_fault 	
	carica_gate	12 		exc_stack_fault 	
	carica_gate	13 		exc_prot_fault 	
	carica_gate	14 		exc_int_tipo_pf 	
	carica_gate	15		exc_unknown_exc	
	carica_gate	16 		exc_fp 		
	carica_gate	17 		exc_ac 		
	carica_gate	18 		exc_mc 		
	carica_gate	19 		exc_simd 	
	carica_gate	0x20 		ignore_pic 	
	carica_gate	0x21 		ignore_pic 	
	carica_gate	0x22 		ignore_pic 	
	carica_gate	0x23 		ignore_pic 	
	carica_gate	0x24 		ignore_pic 	
	carica_gate	0x25 		ignore_pic 	
	carica_gate	0x26 		ignore_pic 	
	carica_gate	0x27 		ignore_pic 	
	carica_gate	0x28 		ignore_pic 	
	carica_gate	0x29 		ignore_pic 	
	carica_gate	0x2A 		ignore_pic 	
	carica_gate	0x2B 		ignore_pic 	
	carica_gate	0x2C 		ignore_pic 	
	carica_gate	0x2D 		ignore_pic 	
	carica_gate	0x2E 		ignore_pic 	
	carica_gate	0x2F 		ignore_pic 	

        lidt    idt_pointer                // carichiamo la nuova IDT

        popq %rdi
	.cfi_adjust_cfa_offset -8
        popq %rax
	.cfi_adjust_cfa_offset -8
        popq %rcx
	.cfi_adjust_cfa_offset -8
        ret
	.cfi_endproc

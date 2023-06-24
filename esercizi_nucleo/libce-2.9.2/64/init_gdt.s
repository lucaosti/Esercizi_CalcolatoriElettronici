#include "def.h"

.global init_gdt
init_gdt:
	.cfi_startproc
	pushq %rdx
	.cfi_adjust_cfa_offset 8
	pushq %rax
	.cfi_adjust_cfa_offset 8
	pushq %rcx
	.cfi_adjust_cfa_offset 8

	movabsq $gdt, %rdx
        addq $SEL_SYS_TSS, %rdx
	movabsq $sys_tss_end, %rcx
	movabsq $sys_tss, %rax
	subq %rax, %rcx
	subq $1, %rcx
	movw %cx, (%rdx) 	//[15:0] = limit[15:0]
	movw %ax, 2(%rdx)	//[31:16] = base[15:0]
	shrq $16,%rax
	movb %al, 4(%rdx)	//[39:32] = base[24:16]
	movb $0b10001001, 5(%rdx)//[47:40] = p_dpl_type
	movb $0, 6(%rdx)	//[55:48] = 0
	movb %ah, 7(%rdx)	//[63:56] = base[31:24]
	shrq $16, %rax
	movl %eax, 8(%rdx) //[95:64] = base[63:32]
	movl $0, 12(%rdx)	//[127:96] = 0

	popq %rcx
	.cfi_adjust_cfa_offset -8
	popq %rax
	.cfi_adjust_cfa_offset -8
	popq %rdx
	.cfi_adjust_cfa_offset -8
        ret
	.cfi_endproc

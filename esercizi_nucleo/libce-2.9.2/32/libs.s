#include "def.h"

.macro carica_descr num base limite pres dpl tipo gran

        pushl $\gran
        pushl $\tipo
        pushl $\dpl
        pushl $\pres
        pushl $\limite
        pushl $\base
        pushl $\num
        call  init_descrittore
        addl $28, %esp

.endm

.text
.global init_descrittore
init_descrittore:
        pushl %ebp
        movl %esp, %ebp

        pushl %ebx
        pushl %ecx
        pushl %eax
        pushl %edx

        movl $gdt, %ebx
        movl 8(%ebp), %ecx              // indice GDT -> %ecx
        movl 16(%ebp), %edx             // limite -> %edx
        movw %dx,  (%ebx, %ecx, 8)      // bit 15:00 limite -> 1a parola descr.
        movw 12(%ebp), %ax              // bit 15:00 base -> %ax
        movw %ax, 2(%ebx, %ecx, 8)      // -> 2a parola descr.
        movb 14(%ebp), %al              // bit 23:16 base -> %al
        orb  24(%ebp), %ah              // DPL
        shlb $5, %ah                    // posizione del DPL nel byte di accesso
        orb  20(%ebp), %ah              // bit di presenza
        orb  28(%ebp), %ah              // tipo
        movw %ax, 4(%ebx, %ecx, 8)      // -> 3a parola descr.
        movb 15(%ebp), %dh              // bit 31:24 base -> %dh
        shrl $16, %edx                  // bit 19:16 limite -> low nibble %dl
        orb  $0b01000000, %dl           // operandi su 32 bit
        orb  32(%ebp), %dl              // granularita'
        movw %dx, 6(%ebx, %ecx, 8)      // -> 4a parola descr.
        
        popl %edx
        popl %eax
        popl %ecx
        popl %ebx

        leave
        ret

.global init_gdt
init_gdt:
        pushl %ecx
        pushl %eax
        pushl %edi

        movl $gdt, %edi
        movl $0x4000, %ecx
        movl $0, %eax
        rep
        stosl

                //indice        base    limite  P       DPL             TIPO            
        carica_descr    0       0       0       NON_P   0               0               0
        carica_descr    1       0       0xfffff PRES    LIV_SISTEMA     SEG_CODICE      G_PAGINA
        carica_descr    2       0       0xfffff PRES    LIV_SISTEMA     SEG_DATI        G_PAGINA
        carica_descr    3       0       0xfffff PRES    LIV_UTENTE      SEG_CODICE      G_PAGINA
        carica_descr    4       0       0xfffff PRES    LIV_UTENTE      SEG_DATI        G_PAGINA
        carica_descr    5       sys_tss 215     PRES    LIV_SISTEMA     SEG_TSS         G_BYTE
        
        popl %ecx
        popl %eax
        popl %edi
        ret
        

.global load_gdt
load_gdt:
        pushl %ecx

        movw    %cs, %cx
        movw    %cx, s_CS
        movw    %ds, %cx
        movw    %cx, s_DS
        movw    %es, %cx
        movw    %cx, s_ES
        movw    %fs, %cx
        movw    %cx, s_FS
        movw    %gs, %cx
        movw    %cx, s_GS
        movw    %ss, %cx
        movw    %cx, s_SS

        sgdt    save_gdt
        lgdt    gdt_pointer             // carichiamo la nuova GDT

        ljmp    $SEL_CODICE_SISTEMA, $qui       // ricarichiamo CS
qui:
        movw    $SEL_DATI_SISTEMA,   %cx   // e gli altri selettori
        movw    %cx, %ss
        movw    %cx, %ds
        movw    %cx, %es
        movw    $0,  %cx                // FS e GS non sono usati
        movw    %cx, %fs
        movw    %cx, %gs

        movw $SEL_SYS_TSS, %cx
        ltr %cx
        
        popl %ecx
        ret

.global unload_gdt
unload_gdt:
        pushl %ecx

        lgdt  save_gdt
        
        pushf
        xorl %ecx, %ecx
        movw  s_CS, %cx
        pushl %ecx
        pushl $avanti
        iret
avanti:
        movw    s_DS, %cx
        movw    %cx, %ds
        movw    s_ES, %cx
        movw    %cx, %es
        movw    s_FS, %cx
        movw    %cx, %fs
        movw    s_GS, %cx
        movw    %cx, %gs
        movw    s_SS, %cx
        movw    %cx, %ss

	popl %ecx
        ret

// carica un gate nella IDT
// parametri: 
// num: indice (a partire da 0) in IDT del gate da caricare
// routine: indirizzo della routine da associare al gate
.global componi_gate
componi_gate:
        pushl %ebp
        movl %esp, %ebp

        pushl %ebx
        pushl %eax

        movl 8(%ebp), %ebx              // indirizzo struttura
        movl 12(%ebp), %eax             // offset della routine

        movw %ax, (%ebx)       		// primi 16 bit dell'offset
        movw $SEL_CODICE_SISTEMA, 2(%ebx)

        movw $0, %ax
        movb $0b10001110, %ah           // byte di accesso
                                        // (presente, 32bit, tipo interrupt)
        movb $LIV_SISTEMA, %al          // DPL
        shlb $5, %al                    // posizione del DPL nel byte di accesso
        orb  %al, %ah                   // byte di accesso con DPL in %ah
        movb $0, %al                    // la parte bassa deve essere 0
        movl %eax, 4(%ebx)     		// 16 bit piu' sign. dell'offset
                                        // e byte di accesso

        popl %eax
        popl %ebx
        leave
        ret

.macro carica_gate num routine dpl

	pushl $\routine
	pushl $\num
	call gate_init
	addl $8, %esp

.endm

.global init_idt
init_idt:
        pushl %ecx
        pushl %eax
        pushl %edi

        movl $idt, %edi
        movl $512, %ecx
        movl $0, %eax
        cld
        rep stosl

	carica_gate	0 		divide_error 	
	carica_gate	1 		debug 		
	carica_gate	2 		nmi 		
	carica_gate	3 		breakpoint 	
	carica_gate	4 		overflow 	
	carica_gate	5 		bound_re 	
	carica_gate	6 		invalid_opcode	
	carica_gate	7 		dev_na 		
	carica_gate	8 		double_fault 	
	carica_gate	9 		coproc_so 	
	carica_gate	10 		invalid_tss 	
	carica_gate	11 		segm_fault 	
	carica_gate	12 		stack_fault 	
	carica_gate	13 		prot_fault 	
	carica_gate	14 		int_tipo_pf 	
	carica_gate	15		unknown_exc	
	carica_gate	16 		fp_exc 		
	carica_gate	17 		ac_exc 		
	carica_gate	18 		mc_exc 		
	carica_gate	19 		simd_exc 	
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

        sidt    save_idt
        lidt    idt_pointer                // carichiamo la nuova IDT

        popl %edi
        popl %eax
        popl %ecx
        ret

divide_error:
	pushl $0
	pushl $0
	jmp comm_exc

debug:
	pushl $0
	pushl $1
	jmp comm_exc

nmi:
	pushl $0
	pushl $2
	jmp comm_exc

breakpoint:
	pushl $0
	pushl $3
	jmp comm_exc

overflow:
	pushl $0
	pushl $4
	jmp comm_exc

bound_re:
	pushl $0
	pushl $5
	jmp comm_exc

invalid_opcode:
	pushl $0
	pushl $6
	jmp comm_exc

dev_na:
	pushl $0
	pushl $7
	jmp comm_exc

double_fault:
	pushl $0 // workaround per bochs
	pushl $8
	jmp comm_exc

coproc_so:
	pushl $0
	pushl $9
	jmp comm_exc

invalid_tss:
	pushl $10
	jmp comm_exc

segm_fault:
	pushl $11
	jmp comm_exc

stack_fault:
	pushl $12
	jmp comm_exc

prot_fault:
	pushl $13
	jmp comm_exc

int_tipo_pf:
	pushl $14
	jmp comm_exc

unknown_exc:
	pushl $0
	pushl $15
	jmp comm_exc

fp_exc:
	pushl $0
	pushl $16
	jmp comm_exc

ac_exc:
	pushl $17
	jmp comm_exc

mc_exc:
	pushl $0
	pushl $18
	jmp comm_exc

simd_exc:
	pushl $0
	pushl $19
	jmp comm_exc


comm_exc:
	call gestore_eccezioni
	hlt

ignore_pic:
	call apic_send_EOI
	iret

.global idt_reset
idt_reset:
    lidt    save_idt
    ret

.global sti
sti:   sti
        ret

.global cli
cli:   cli
        ret

.data
gdt_pointer:
        .word 0xffff                    // limite della GDT
        .long gdt                       // base della GDT
idt_pointer:
        .word 0x7FF                     // limite della IDT (256 entrate)
        .long idt                       // base della IDT
.bss
.balign 16
gdt:
        .space 8 * 8192, 0

.balign 16
.global idt
idt:
        .space 8 * 256, 0
save_selectors:
s_CS:   .word 0
s_DS:   .word 0
s_ES:   .word 0
s_FS:   .word 0
s_GS:   .word 0
s_SS:   .word 0
save_gdt:
        .word 0
        .long 0
save_idt:
        .word 0
        .long 0
sys_tss:
        .space 216, 0

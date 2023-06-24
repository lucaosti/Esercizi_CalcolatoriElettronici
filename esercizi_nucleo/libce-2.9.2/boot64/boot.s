// sistema.s

#define ASM 1
#include "mboot.h"

/////////////////////////////////////////////////////////////////////////
// AVVIO                                                                //
//////////////////////////////////////////////////////////////////////////
// Il bootstrap loader attiva il modo protetto (per poter accedere agli
// indirizzi di memoria principale superiori a 1MiB) e carica il sistema
// in memoria, quindi salta alla prima
// istruzione del sistema. Il bootstrap loader puo' anche passare
// delle informazioni al sistema (tramite i registri e la memoria).
//
// In questo sistema usiamo lo standard multiboot,
// che definisce il formato che 
// il file contentente il sistema deve rispettare e
// il formato delle informazioni passate dal bootstrap loader al sistema.
// Il formato del file contenente il sistema deve essere quello di un
// normale file eseguibile, ma, nei primi 2*4KiB, deve contenere 
// la struttura multiboot_header, definita piu' avanti. 
// Il boot loader, prima di saltare alla prima istruzione del sistema
// (l'entry point specificato nel file eseguibile), lascia nel registro
// %eax un valore di riconoscimento e in %ebx l'indirizzo di una struttura
// dati, contentente varie informazioni (in particolare, la quantita'
// di memoria principale installata nel sistema, il dispositivo da cui
// e' stato eseguito il bootstrap e l'indirizzo di memoria in cui sono
// stati caricati gli eventuali moduli)
     .text

#define STACK_SIZE                      0x4000

#ifdef __ELF__
#	define MULTIBOOT_HEADER_FLAGS         (0x00000003)
#else
# 	define MULTIBOOT_HEADER_FLAGS         (0x00010003)
#endif



     .globl  _start, start
_start:				// entry point
start:
     jmp     multiboot_entry	// scavalchiamo la struttra richiesta
     				// dal bootstrap loader, che deve
				// trovarsi verso l'inizio del file

     .align  4
     // le seguenti informazioni sono richieste dal bootstrap loader
multiboot_header:
     .long   MULTIBOOT_HEADER_MAGIC 			// valore magico
     .long   MULTIBOOT_HEADER_FLAGS   				// flag
     .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) // checksum
     .long   multiboot_header
     .long   _start
     .long   _edata
     .long   _end
     .long   multiboot_entry

multiboot_entry:

     cli
     movl    $(stack + STACK_SIZE), %esp // inizializziamo lo stack

     pushl   %ebx			// parametri passati dal loader
     pushl   %eax			
     call    cmain			// il resto dell'inizializzazione
     					// e' scritto in C++

//////////////////////////////////////////////////////////////////
// funzioni di utilita'						//
//////////////////////////////////////////////////////////////////

	
// attiva la paginazione
	.global attiva_paginazione
attiva_paginazione:
	cli

	movl 8(%esp), %eax
	subl $jmp_off_rel, %eax
	movl %eax, jmp_off

	movl %cr4, %eax
        orl $0x00000020, %eax  //cr4.pae
	cmpl $4, 12(%esp)
	je 1f
	orl $0x00001000, %eax	// cr4.la57
1:	 movl %eax, %cr4

        movl $0xC0000080, %ecx   //efer msr
        rdmsr
        orl $0x00000100, %eax   //efer.lme
        wrmsr

        movl %cr0, %eax
        orl $0x80000000, %eax   //cr0.pg
        movl %eax, %cr0
	lgdt gdt64_ptr
	ljmp $8, $long_mode

.code64
long_mode:
	pushq $0
	popfq
	movw $0, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

wait:
	cmpl $1, wait_for_gdb(%rip)
	je wait

	xorq %rdi, %rdi
	movl 4(%rsp), %edi
	movl $stack, %esp
	subq $8, %rsp
	movq $0, (%rsp)

	.byte 0xe9
jmp_off:
	.long 0
jmp_off_rel:
	nop
.code32

	

////////////////////////////////////////////////////////////////
// sezione dati: tabelle e stack			      //
////////////////////////////////////////////////////////////////
.data
.balign 4096
gdt64:
	.quad 0		//segmento nullo
	.quad 0x209c0000000000 //segmento codice sistema
gdt64_end:
gdt64_ptr:
	.word gdt64_end - gdt64
	.long gdt64


.align 16
	.global stack
stack:
	.space STACK_SIZE, 0

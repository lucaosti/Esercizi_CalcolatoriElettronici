/*
Struttura della classe
+-------------------+
| clc_s | XXXXXXXXX | 0
+-------------------+
|       clc_v       | 8
+-------------------+
|       clc_v       | 16
+-------------------+
*/

.set clc_s, 0
.set clc_v, 8

/*
Struttura del costruttore
+-------------------+
| XXXXXXXXXXXXXXXXX | -32
+-------------------+
|       ptr_s2      | -24
+-------------------+
|    i    | XXX | c | -16
+-------------------+
|        this       | -8
+-------------------+
|    vecchio_rbp    | 0
+-------------------+
*/

.set this, -8
.set i, -12
.set c, -16
.set ptr_s2, -24

.global _ZN2clC1EcR3st1

_ZN2clC1EcR3st1:
    pushq %rbp
   	movq %rsp, %rbp
   	subq $32, %rsp

    movq %rdi, this(%rbp)
    movb %sil, c(%rbp)
    movq %rdx, ptr_s2(%rbp)

    movl $0, i(%rbp)
    for_c:
        cmpl $2, i(%rbp)
        je fine_for_c
        movslq i(%rbp), %rcx

        movb c(%rbp), %al
        movq this(%rbp), %rdx
        movb %al, clc_s(%rdx, %rcx, 1)

        movq ptr_s2(%rbp), %rbx
        xorq %rax, %rax
        movb (%rbx, %rcx, 1), %al
        subb c(%rbp), %al
        movq %rax, clc_v(%rdx, %rcx, 8)

        incl i(%rbp)
        jmp for_c
    fine_for_c:
    leave
    ret

/*
Struttura della elab1
+-------------------+
| XXXXXXX |    i    | -48
+-------------------+
|       ptr_s1      | -40
+-------------------+
|        cla        | -32
+-------------------+
|        cla        | -24
+-------------------+
|        cla        | -16
+-------------------+
|       this        | -8
+-------------------+
|    vecchio_rbp    | 0
+-------------------+
*/

.set this, -8
.set cla, -32
.set ptr_s1, -40
.set i, -48

.global _ZN2cl5elab1ER3st1

_ZN2cl5elab1ER3st1:
    pushq %rbp
   	movq %rsp, %rbp
   	subq $48, %rsp

    movq %rdi, this(%rbp)
    movq %rsi, ptr_s1(%rbp)

    leaq cla(%rbp), %rdi
    movb $'p', %sil
    movq ptr_s1(%rbp), %rdx

    call _ZN2clC1EcR3st1

    movl $0, i(%rbp)
    for_e:
        cmpl $2, i(%rbp)
        je end_for_e
        movslq i(%rbp), %rcx

        movq ptr_s1(%rbp), %rdx
        movb (%rdx, %rcx, 1), %al
        movq this(%rbp), %rdx
        cmpb %al, clc_s(%rdx, %rcx, 1)
        jge noif

        movb cla+clc_s(%rbp, %rcx, 1), %al
        movb %al, clc_s(%rdx, %rcx, 1)

        movq cla+clc_v(%rbp, %rcx, 8), %rax
        addq %rcx, %rax
        movq %rax, (%rdx, %rcx, 8)

        noif:
        incl i(%rbp)
        jmp for_e
    end_for_e:
    leave
    ret
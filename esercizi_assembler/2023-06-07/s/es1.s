/*
Struttura della classe
+-------------------+
|  vc  | XXXXXXXXXX | 0
+-------------------+
|         v         | 8
+-------------------+
|         v         | 16
+-------------------+
|         v         | 24
+-------------------+
*/

.set clc_vc, 0
.set clc_v, 8

/*-----------------*/

/*
Struttura della elab1
+-------------------+
|    i    | XXXXXXX | -48
+-------------------+
|        cla        | -40
+-------------------+
|        cla        | -32
+-------------------+
|        cla        | -24
+-------------------+
|      ptr_s1       | -16
+-------------------+
|       this        | -8
+-------------------+
|    vecchio_rbp    | 0
+-------------------+
*/

.set this, -8
.set ptr_s1, -16
.set cla, -40
.set i, -44

.global _ZN2cl5elab1ER3st1

_ZN2cl5elab1ER3st1:
    # prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp # Allineo a 16

    # Copio nello stack
    movq %rdi, this(%rbp)
    movq %rsi, ptr_s1(%rbp)

    # Preparo i registri per la chiamata al costruttore
    leaq cla(%rbp), %rdi
    movb $'q', %sil
    movq ptr_s1(%rbp), %rdx

    CALL _ZN2clC1EcR3st1

    movl $0, i(%rbp)
    for:
        cmpl $3, i(%rbp)
        je fine_for
        movslq i(%rbp), %rcx

        movb (%rdx, %rcx, 1), %al
        movq this(%rbp), %rdx
        cmpb %al, clc_vc(%rdx, %rcx, 1)
        jl noif

            movb cla+clc_vc(%rbp, %rcx, 1), %al
            movb %al, clc_vc(%rdx, %rcx, 1)

            movq cla+clc_v(%rbp, %rcx, 8), %rax
            movq %rax, clc_v(%rdx, %rcx, 8)
            subq %rcx, clc_v(%rdx, %rcx, 8)

        noif:
        incl i(%rbp)
        jmp for
    fine_for:
    # epilogo
    leave
    ret
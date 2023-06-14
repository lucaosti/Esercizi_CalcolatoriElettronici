/*
Struttura della classe
    0  1  2  3  4  5  6  7
   +----------------------+
0  |      classe_vv2      |
   +----------------------+
8  |      classe_vv2      |
   +----------------------+
16 |      classe_vv2      |
   +----------------------+
24 |      classe_vv2      |
   +----------------------+
32 | classe_vv1 | XXXXXXX |
   +----------------------+
*/

.set classe_vv2, 0
.set classe_vv1, 32

.global _ZN2clC1EPc

/*
Struttura del costruttore
+----------------------+
| XXXXXXXXXXXXXXXXXXXX | <- -32
+----------------------+
| XXXXXXXX |     i     | <- -24
+----------------------+
|        ptr_v         | <- -16
+----------------------+
|         this         | <- -8
+----------------------+
|     vecchio_rbp      | <- -0
+----------------------+
*/

.set this, -8
.set ptr_v, -16
.set i, -24

_ZN2clC1EPc:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp # Allineo a 16

    # Copia dei parametri
    movq %rdi, this(%rbp)
    movq %rsi, ptr_v(%rbp)

    movl $0, i(%rbp)
    for1:
        cmpl $4, i(%rbp)
        je finefor1
        movslq i(%rbp), %rcx

        # Dentro il for
        xorq %rax, %rax
        movq ptr_v(%rbp), %rdx
        movb (%rdx, %rcx, 1), %al
        movq this(%rbp), %rdx
        movb %al, classe_vv1(%rdx, %rcx, 1)
        movq %rax, classe_vv2(%rdx, %rcx, 8)

        incl i(%rbp)
        jmp for1
    finefor1:
    leave
    ret

.global _ZN2cl5elab1ER2sti

/*
Struttura della elab
+----------------------+
| XXXXXXXXXXXXXXXXXXXX | <- -32
+----------------------+
|    i     |     d     | <- -24
+----------------------+
|        ptr_ss        | <- -16
+----------------------+
|         this         | <- -8
+----------------------+
|     vecchio_rbp      | <- -0
+----------------------+
*/

.set this, -8
.set ptr_ss, -16
.set i, -20
.set d, -24

_ZN2cl5elab1ER2sti:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp # Allineo a 16

    # Copia dei parametri
    movq %rdi, this(%rbp)
    movq %rsi, ptr_ss(%rbp)
    movl %edx, d(%rbp)

    movl $0, i(%rbp)
    for2:
        cmpl $4, i(%rbp)
        je finefor2
        movslq i(%rbp), %rcx

        movslq d(%rbp), %rax
        movq ptr_ss(%rbp), %rdx
        cmpq %rax, classe_vv2(%rdx, %rcx, 8)
        jg fineif

        # Dentro if
        movq this(%rbp), %rbx
        movb classe_vv1(%rbx, %rcx, 1), %al
        subb classe_vv1(%rdx, %rcx, 1), %al
        movb %al, classe_vv1(%rbx, %rcx, 1)

        fineif:
        movq %rcx, %rax
        movslq d(%rbp), %rdx
        subq %rdx, %rax
        movq %rax, classe_vv2(%rbx, %rcx, 8)

        incl i(%rbp)
        jmp for2
    finefor2:
    leave
    ret

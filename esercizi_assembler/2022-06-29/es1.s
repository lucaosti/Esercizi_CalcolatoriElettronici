/*
Struttura della classe
+--------------------+
|  classe_vv1  | XXX |
+--------------------+
|     classe_vv2     |
+--------------------+
|     classe_vv2     |
+--------------------+
|     classe_vv2     |
+--------------------+
*/

.set classe_vv1, 0
.set classe_vv2, 8

.global _ZN2clC1EPc

/*
Struttura del costruttore
+--------------------+
| XXXXXXXXXXXXXXXXXX |
+--------------------+
| XXXXXXXX |    i    |
+--------------------+
|        ptr_v       |
+--------------------+
|        this        |
+--------------------+
|     vecchio rbp    |
+--------------------+
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
        cmpl $3, i(%rbp)
        je finefor1
        movslq i(%rbp), %rcx

        movq ptr_v(%rbp), %rdx
        movsbq (%rdx, %rcx, 1), %rax

        movq this(%rbp), %rdx

        movq %rax, classe_vv2(%rdx, %rcx, 8)
        movb %al, classe_vv1(%rdx, %rcx, 1)
        movb %al, classe_vv1+3(%rdx, %rcx, 1)

        incl i(%rbp)
        jmp for1
    finefor1:
        leave
        ret

/***********************************************/

.global _ZN2cl5elab1EiR2st

/*
Struttura della elab
+--------------------+
| XXXXXXXXXXXXXXXXXX |
+--------------------+
|    d     |    i    |
+--------------------+
|        ptr_ss      |
+--------------------+
|        this        |
+--------------------+
|     vecchio rbp    |
+--------------------+
*/

.set this, -8
.set ptr_ss, -16
.set d, -20
.set i, -24

_ZN2cl5elab1EiR2st:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp # Allineo a 16

    # Copia dei parametri
    movq %rdi, this(%rbp)
    movb %sil, d(%rbp)
    movq %rdx, ptr_ss(%rbp)

    movl $0, i(%rbp)
    for2:
        cmpl $3, i(%rbp)
        je finefor2

        movq ptr_ss(%rbp), %rdx
        movslq d(%rbp), %rax
        cmpq %rax, classe_vv2(%rdx, %rcx, 8)
        jl else

        # Dentro if
        movb classe_vv1(%rdx, %rcx, 1), %bl
        movq this(%rbp), %rdx
        addb %bl, classe_vv1(%rdx, %rcx, 1)

        subq %rax, classe_vv2(%rdx, %rcx, 8)

        jmp fineif
        # Else
    else:
        movq ptr_ss(%rbp), %rbx
        movb classe_vv1(%rbx, %rcx, 1), %al
        subb %al, classe_vv1+3(%rdx, %rcx, 1)

        movslq d(%rbp), %rax
        addq %rax, classe_vv2(%rdx, %rcx, 8)
        # Fine if
    fineif:
        incl i(%rbp)
        jmp for2
    finefor2:
    leave
    ret

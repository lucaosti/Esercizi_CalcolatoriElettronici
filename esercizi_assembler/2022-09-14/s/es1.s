/*
Struttura della classe
    +---------------------+
 0  |    c1    |    c2    |
    +---------------------+
 8  |          v          |
    +---------------------+
 16 |          v          |
    +---------------------+
 24 |          v          |
    +---------------------+
 32 |          v          |
    +---------------------+
*/
.set classe_c1, 0
.set classe_c2, 4
.set classe_v, 8

/*-----------------------*/

.global _ZN2clC1EcR3st1

/*
Struttura del costruttore
+---------------------+
| XXXXXXXXXXXXXXXXXXX | <- -32
+---------------------+
|       ptr_s2        | <- -24
+---------------------+
|     i    | XXXX | c | <- -16
+---------------------+
|        this         | <- -8
+---------------------+
|     vecchio rbp     | <- -0
+---------------------+
*/

.set this, -8
.set i, -12
.set c, -16
.set ptr_s2, -24

_ZN2clC1EcR3st1:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp # Allineo a 16

    # Copio i parametri nello stack
    movq %rdi, this(%rbp)
    movb %sil, c(%rbp)
    movq %rdx, ptr_s2(%rbp)

    movl $0, i(%rbp)
    for1:
        cmpl $4, i(%rbp)
        je finefor1
        movslq i(%rbp), %rcx

        # Dentro il for
        # c1.vc[i] = c2.vc[i] = c;
        movb c(%rbp), %al
        movq this(%rbp), %rdx
        movb %al, classe_c1(%rdx, %rcx, 1)
        movb %al, classe_c2(%rdx, %rcx, 1)

        # v[i] = s2.vd[i] - c2.vc[i];
        movq ptr_s2(%rbp), %rdx
        movslq (%rdx, %rcx, 4), %rax
        movq this(%rbp), %rdx
        movsbq (%rdx, %rcx, 1), %rbx
        subq %rbx, %rax
        movq %rax, classe_v(%rdx, %rcx, 8)

        incl i(%rbp)
        jmp for1
    finefor1:
    leave
    ret

/*-----------------------*/

.global _ZN2cl5ebal1E3st13st2

/*
Struttura della elab
+---------------------+
|       classe        | <- -64
+---------------------+
|       classe        | <- -56
+---------------------+
|       classe        | <- -48
+---------------------+
|       classe        | <- -40
+---------------------+
|    s2    |    s2    | <- -32
+---------------------+
|    s2    |    s2    | <- -24
+---------------------+
|    i     |    s1    | <- -16
+---------------------+
|        this         | <- -8
+---------------------+
|     vecchio rbp     | <- -0
+---------------------+
*/

.set this, -8
.set i, -12
.set s1, -16
.set s2, -32
.set classe, -64

_ZN2cl5ebal1E3st13st2:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $64, %rsp # Allineo a 16

    # Copio i parametri nello stack
    movq %rdi, this(%rbp)
    movl %esi, s1(%rbp)
    movq %rdx, s2(%rbp)
    movq %rcx, s2+8(%rbp)

    # Preparo i paramentri per il costruttore
    leaq classe(%rbp), %rdi
    movb $'z', %sil
    leaq s2(%rbp), %rdx

    # Chiamo il costruttore
    call _ZN2clC1EcR3st1

    movl $0, i(%rbp)
    for2:
        cmp $4, i(%rbp)
        je finefor2

        # parametri if
        movq this(%rbp), %rdx
        movb s1(%rbp), %al
        cmpb %al, classe_c2(%rdx, %rcx, 1)
        jge fineif

        # Dentro if
        movb classe+classe_c2(%rbp, %rcx, 1), %al
        movb %al, classe_c1(%rdx, %rcx, 1)
        movq classe+classe_v(%rbp, %rcx, 8), %rax
        movq %rax, classe_v(%rdx, %rcx, 8)

        fineif:

        incl i(%rbp)
        jmp for2
    finefor2:
    leave
    ret

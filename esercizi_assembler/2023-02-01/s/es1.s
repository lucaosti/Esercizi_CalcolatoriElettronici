/*
    Struttura della classe
    0  1  2  3  4  5  6  7
    +--------------------+
 0  |     classe_v2      |
    +--------------------+
 8  |     classe_v2      |
    +--------------------+
16  |     classe_v2      |
    +--------------------+
24  |     classe_v2      |
    +--------------------+
32  |classe_v1|classe_v3 |
    +--------------------+
*/

.set classe_v2, 0
.set classe_v1, 32
.set classe_v3, 36

.global _ZN2clC1ER3st1

/*
Stack del primo costruttore
7  6  5  4  3  2  1  0
+--------------------+
| XXXXXXXXXXXXXXXXXX | <- -32
+--------------------+
|    i    | XXXXXXXX | <- -24
+--------------------+
|         ss         | <- -16
+--------------------+
|        this        | <- -8
+--------------------+
|     vecchio rbp    | <- 0
+--------------------+
*/

.set this, -8
.set ss, -16
.set i, -28

_ZN2clC1ER3st1:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp

    # Copio i parametri
    movq %rdi, this(%rbp)
    movq %rsi, ss(%rbp)

    # Inizializzare i
    movl $0, i(%rbp)
    for:
        cmpl $4, i(%rbp)
        je fine_for

        movslq i(%rbp), %rcx # Uso rcx come i
        
        movq ss(%rbp), %rdx
        # ss.vi[i]
        movl (%rdx, %rcx, 4), %eax
        
        movq this(%rbp), %rdx # v1[i]
        movb %al, classe_v1(%rdx, %rcx, 1)
        sal $1, %eax # *2
        movslq %eax, %rax
        movq %rax, classe_v2(%rdx, %rcx, 8)
        sal $1, %eax # *4
        movb %al, classe_v3(%rdx, %rcx, 1)
        
        incl i(%rbp)
        jmp for
    fine_for:
    
    # Epilogo
    leave
    ret

.global _ZN2clC1ER3st1Pi

/*
Stack del secondo costruttore
7  6  5  4  3  2  1  0
+--------------------+
|    i    | XXXXXXXX | <- -32
+--------------------+
|         ar2        | <- -24
+--------------------+
|         s1         | <- -16
+--------------------+
|        this        | <- -8
+--------------------+
|     vecchio rbp    | <- 0
+--------------------+
*/

.set this, -8
.set s1, -16
.set ar2, -24
.set i, -28

_ZN2clC1ER3st1Pi:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp

    # Copio i parametri
    movq %rdi, this(%rbp)
    movq %rsi, s1(%rbp)
    movq %rdx, ar2(%rbp)

    # Inizializzare i
    movl $0, i(%rbp)
    for2:
        cmpl $4, i(%rbp)
        je fine_for2

        movslq i(%rbp), %rcx # Uso rcx come i
        
        movq ss(%rbp), %rdx
        # ss.vi[i]
        movl (%rdx, %rcx, 4), %eax
        
        movq this(%rbp), %rdx # v1[i]
        movb %al, classe_v1(%rdx, %rcx, 1)
        sal $3, %eax # *8
        movslq %eax, %rax
        movq %rax, classe_v2(%rdx, %rcx, 8)
        # Uso ar2 per v3
        movq ar2(%rbp), %rax
        movb (%rax, %rcx, 4), %al
        movb %al, classe_v3(%rdx, %rcx, 1)
        
        incl i(%rbp)
        jmp for2
    fine_for2:
    
    # Epilogo
    leave
    ret

.global _ZN2cl5elab1EPKc3st2

/*
Stack del secondo costruttore
7  6  5  4  3  2  1  0
+--------------------+
|     classe_ptr     | <- -48
+--------------------+
|         s1         | <- -40
+--------------------+
|         s1         | <- -32
+--------------------+
|    i    |    s2    | <- -24
+--------------------+
|        ar1         | <- -16
+--------------------+
|        this        | <- -8
+--------------------+
|     vecchio rbp    | <- 0
+--------------------+
*/

.set this, -8
.set ar1, -16
.set i, -20
.set s2, -24
.set s1, -40
.set classe_ptr, -48

_ZN2cl5elab1EPKc3st2:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp

    # Copio i parametri
    movq %rdi, classe_ptr(%rbp)
    movq %rsi, this(%rbp)
    movq %rdx, ar1(%rbp)
    movl %ecx, s2(%rbp)

    movl $0, i(%rbp)
    forf1:
        cmpl $4, i(%rbp)
        je fine_forf1
        movslq i(%rbp), %rcx

        # s1.vi[i] = ar1[i] + i;
        movq $0, %rax
        movq ar1(%rbp), %rdx
        movb (%rdx, %rcx, 1), %al
        addq %rcx, %rax
        movl %eax, s1(%rbp, %rcx, 4)

        incl i(%rbp)
        jmp forf1
    fine_forf1:

    # Preparo i registri per la chiamata del costruttore
    movq classe_ptr(%rbp), %rdi
    leaq s1(%rbp), %rsi

    # Chiama il costruttore
    call _ZN2clC1ER3st1

    movl $0, i(%rbp)
    forf2:
        cmpl $4, i(%rbp)
        je fine_forf2
        movslq i(%rbp), %rcx

        # cla.v3[i] = s2.vd[i];
        movb s2(%rbp, %rcx, 1), %al
        movq classe_ptr(%rbp), %rdx
        movb %al, classe_v3(%rdx, %rcx, 1)

        incl i(%rbp)
        jmp forf2
    fine_forf2:

        leave
        ret

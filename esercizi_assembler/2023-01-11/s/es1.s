/*
Struttura della classe
      0 1 2 3 4 5 6 7
      +-------------+
0  -> | cl_s | cl_s |
      +-------------+
8  -> | cl_s | cl_s |
      +-------------+
16 -> |    cl_v     |
      +-------------+
24 -> |    cl_v     |
      +-------------+
32 -> |    cl_v     |
      +-------------+
40 -> |    cl_v     |
      +-------------+
*/

.set cl_s, 0
.set cl_v, 16


.global _ZN2clC1Ec3st2
/*
Pila del costruttore
 7  6  5  4  3  2  1  0
+----------------------+
|    s2    |     s2    | <- -32
+----------------------+
|    s2    |     s2    | <- -24
+----------------------+
| XXXX | c |     i     | <- -16
+----------------------+
|         this         | <- -8
+----------------------+
|      vecchio rbp     | <- -0
+----------------------+
*/

.set s2, -32
.set c, -12
.set i, -16
.set this, -8

_ZN2clC1Ec3st2:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp # Allineo a 16

    # Copio nello stack
    movq %rdi, this(%rbp)
    movb %sil, c(%rbp)
    movq %rdx, s2(%rbp)
    movq %rcx, s2+8(%rbp)

    movl $0, i(%rbp)
    for1:
        # Condizione for1
        cmpl $4, i(%rbp)
        je finefor1
        movslq i(%rbp), %rcx

        # Dentro il for1
        
        # s.vd[i] = c;
        xor %rax, %rax
        movq this(%rbp), %rdx
        movb c(%rbp), %al
        movl %eax, cl_s(%rdx, %rcx, 4)

        # v[i] = s2.vd[i] + s.vd[i];
        movl cl_s(%rdx, %rcx, 4), %eax # s.vd[i]
        addl s2(%rbp, %rcx, 4), %eax # + s2.vd[i]
        movq %rax, cl_v(%rdx, %rcx, 8) # Assegnamento

        # Iterazione for1
        incl i(%rbp)
        jmp for1
    finefor1:
    leave
    ret

.global _ZN2cl5elab1E3st1R3st2

/*
Pila di elab1
 7  6  5  4  3  2  1  0
+----------------------+
| XXXXXXXXXXXXXXXXXXXX | <- -80
+----------------------+
|        classe        | <- -72
+----------------------+
|        classe        | <- -64
+----------------------+
|        classe        | <- -56
+----------------------+
|        classe        | <- -48
+----------------------+
|        classe        | <- -40
+----------------------+
|        classe        | <- -32
+----------------------+
|        ptr_s2        | <- -24
+----------------------+
|    s1    |     i     | <- -16
+----------------------+
|         this         | <- -8
+----------------------+
|      vecchio rbp     | <- -0
+----------------------+
*/

.set this, -8
.set i, -16
.set s1, -12
.set ptr_s2, -24
.set classe, -72

_ZN2cl5elab1E3st1R3st2:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $80, %rsp # Allineo a 16

    # Copio nello stack
    movq %rdi, this(%rbp)
    movl %esi, s1(%rbp)
    movq %rdx, ptr_s2(%rbp)

    # Preparo i registri per la chiamata del costruttore
    leaq classe(%rbp), %rdi
    movb s1+3(%rbp), %sil
    movq ptr_s2(%rbp), %rax
    movq (%rax), %rdx
    movq 8(%rax), %rcx

    call _ZN2clC1Ec3st2

    movl $0, i(%rbp)
    for2:
        # Condizione for2
        cmpl $4, i(%rbp)
        je finefor2
        movslq i(%rbp), %rcx

        # Dentro il for2
        # Condizione if1
        movq this(%rbp), %rdx
        movl (%rdx, %rcx, 4), %eax # s.vd[i]
        movsbl s1(%rbp, %rcx, 1), %ebx
        cmpl %eax, %ebx
        jl fineif1

        # Dentro if1
        movl classe+cl_s(%rbp, %rcx, 4), %eax
        movl %eax, (%rdx, %rcx, 4)

    fineif1:
        # Condizione if2
        movq this(%rbp), %rdx
        movq classe+cl_v(%rbp, %rcx, 8), %rbx
        cmpq cl_v(%rdx, %rcx, 8), %rbx
        jle fineif2

        # Dentro if2
        addq %rbx, cl_v(%rdx, %rcx, 8)

    fineif2:
        # Iterazione for2
        incl i(%rbp)
        jmp for2
    finefor2:
        leave
        ret

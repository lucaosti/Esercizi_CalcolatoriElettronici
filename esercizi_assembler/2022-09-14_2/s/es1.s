/*
Struttura della classe
+---------------------+
|  clc_c1  |  clc_c2  | 0
+---------------------+
|        clc_v        | 8
+---------------------+
|        clc_v        | 16
+---------------------+
|        clc_v        | 24
+---------------------+
|        clc_v        | 32
+---------------------+
*/

.set clc_c1,    0
.set clc_c2,    4
.set clc_v,     8

.global _ZN2clC1EcR3st2

/*
Struttura del costruttore
+---------------------+
| XXXXXXXXXXXXXXXXXXX | -32
+---------------------+
|       ptr_s2        | -24
+---------------------+
|     i    | XXXX | c | -16
+---------------------+
|        this         | -8
+---------------------+
|     vecchio_rbp     | 0
+---------------------+
*/

.set this, -8
.set i, -12
.set c, -16
.set ptr_s2, -24

_ZN2clC1EcR3st2:
    pushq %rbp
   	movq %rsp, %rbp
   	subq $32, %rsp

    movq %rdi, this(%rbp)
    movb %sil, c(%rbp)
    movq %rdx, ptr_s2(%rbp)

    movl $0, i(%rbp)
    for_c:
        cmpl $4, i(%rbp)
        je fine_for_c
        movslq i(%rbp), %rcx

        movb %sil, clc_c1(%rdi, %rcx, 1)
        movb %sil, clc_c2(%rdi, %rcx, 1)

        xorq %rax, %rax
        movb (%rdx, %rcx, 1), %al
        subb clc_c2(%rdi, %rcx, 1), %al

        movq %rax, clc_v(%rdi, %rcx, 8)

        incl i(%rbp)
        jmp for_c
    fine_for_c:
    leave
    ret

.global _ZN2cl5elab1E3st13st2

/*
Struttura della elab1
+---------------------+
| xXXXXXXX |    i     | -64
+---------------------+
|    s1    |    s2    | -56
+---------------------+
|         cla         | -48
+---------------------+
|         cla         | -40
+---------------------+
|         cla         | -32
+---------------------+
|         cla         | -24
+---------------------+
|         cla         | -16
+---------------------+
|         this        | -8
+---------------------+
|     vecchio_rbp     | 0
+---------------------+
*/

.set this, -8
.set cla, -48
.set s1, -44
.set s2, -48
.set i, -64

_ZN2cl5elab1E3st13st2:
    pushq %rbp
   	movq %rsp, %rbp
   	subq $64, %rsp

    movq %rdi, this(%rbp)
    movl %esi, s1(%rbp)
    movl %edx, s1(%rbp)
prova:
    leaq cla(%rbp), %rdi
    movb $'z', %sil
    leaq s2(%rbp), %rdx

    call _ZN2clC1EcR3st2

    movl $0, i(%rbp)
    for_e:
        cmpl $4, i(%rbp)
        je fine_for_e
        movq i(%rbp), %rcx

        movq this(%rbp), %r8

        movb clc_c2(%r8, %rcx, 1), %al
        cmp %al, s1(%rbp, %rcx, 1)
        jle noif

        movb cla+clc_c2(%rbp, %rcx, 1), %al
        movb %al, clc_c1(%r8, %rcx, 1)

        movq cla+clc_v(%rbp, %rcx, 8), %rax
        movq %rax, clc_v(%r8, %rcx, 1)

        noif:
        incl i(%rbp)
        jmp for_e
    fine_for_e:
    leave
    ret
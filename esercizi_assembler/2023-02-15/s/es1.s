/*  Struttura della classe
      0 1 2 3 4 5 6 7
    +------------------+
0   |classe_s|  XXXXXX |
    +------------------+
8   |     classe_v     |
    +------------------+
16  |     classe_v     |
    +------------------+
24  |     classe_v     |
    +------------------+
32  |     classe_v     |
    +------------------+
*/

.set classe_s, 0
.set classe_v, 8

.global _ZN2clC1EPc3st1

/* Stack del costruttore
 7 6 5 4 3 2 1 0
+----------------+
| XXXXXXXXXXXXXX | <- -32
+----------------+
|   s2   |   i   | <- -24
+----------------+
|     ptr_c      | <- -16
+----------------+
|      this      | <- -8
+----------------+
|  Vecchio rbp   | <- 0
+----------------+
*/

.set this, -8
.set ptr_c, -16
.set s2, -20
.set i, -24

_ZN2clC1EPc3st1:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp # Allineo a 16

    # Copio i parametri nello stack
    movq %rdi, this(%rbp)
    movq %rsi, ptr_c(%rbp)
    movl %edx, s2(%rbp)

    # Traduzione
    movq this(%rbp), %rdi # Non necessario, per sicurezza
    movq ptr_c(%rbp), %rdx
    movl $0, i(%rbp)
    for:
        # Condizione del for
        cmpl $4, i(%rbp)
        JE fine_for
        
        # Dentro il for va sempre fatto
        movslq i(%rbp), %rcx

        movb (%rdx), %al

        # s.vc[i] = *c;
        movb %al, classe_s(%rdi, %rcx, 1)   # In rdi ho this, in rcx la i
        
        # v[i] = s2.vc[i] - *c;
        movb s2(%rbp, %rcx, 1), %bl
        subb %al, %bl
        movsbq %bl, %rbx
        movq  %rbx, classe_v(%rdi, %rcx, 8)

        incl i(%rbp)

        jmp for
    fine_for:

    # Epilogo
    leave
    ret

# ---------------------------- #

.global _ZN2cl5elab1ER3st1

/* Stack dell'elab1
 7 6 5 4 3 2 1 0    
+------------------+
|XXXXXXX| classe_s | <- -64
+------------------+
|     classe_v     | <- -56
+------------------+
|     classe_v     | <- -48
+------------------+
|     classe_v     | <- -40
+------------------+
|     classe_v     | <- -32
+------------------+
|   i    | XXXXXXX | <- -24
+------------------+
|        s1        | <- -16
+------------------+
|       this       | <- -8
+------------------+
|   Vecchio rbp    | <- 0
+------------------+
*/

.set this, -8
.set s1, -16
.set i, -20
.set classe, -64

_ZN2cl5elab1ER3st1:
    # Prologo
    pushq %rbp
    movq %rsp, %rbp
    subq $64, %rsp # Allineo a 16

    # Copio i parametri nello stack
    movq %rdi, this(%rbp)
    movq %rsi, s1(%rbp)

    # Inserisco i valori nei registri per chiamare il costruttore
    leaq classe(%rbp), %rdi # this del costruttore
    movq this(%rbp), %rsi # Il riferimento al primo elemento è scritto nello stack all'indirizzo this(%rbp)
    movq s1(%rbp), %rdx
    movl (%rdx), %edx # Lo prendo per valore

    call _ZN2clC1EPc3st1

    movl $0, i(%rbp)
    for2:
        # Condizione
        cmpl $4, i(%rbp)
        je fine_for2

        movslq i(%rbp), %rcx # Uso rcx per il displacement

        # Dentro for
        movq this(%rbp), %rdx
        movb (%rdx, %rcx, 1), %al
        movq s1(%rbp), %rdx

        cmpb (%rdx, %rcx, 1), %al
        jge epilogo_for
        
        # Dentro if
        # s.vc[i] = cla.s.vc[i];
        movq this(%rbp), %rdx
        movb classe(%rbp, %rcx, 1), %al
        movb %al, (%rdx, %rcx, 1)

        # v[i] = cla.v[i] + i;
        movq classe+classe_v(%rbp, %rcx, 8), %rax
        addq %rcx, %rax
        movq %rax, classe_v(%rdx, %rcx, 8)
    epilogo_for:    
        incl i(%rbp)
        jmp for2
    fine_for2:

    # Epilogo
    leave
    ret

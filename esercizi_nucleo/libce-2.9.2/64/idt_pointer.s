#include "def.h"
.data
.balign 16
.global idt_pointer
idt_pointer:
        .word 0xFFF                     // limite della IDT (256 entrate)
        .quad idt                       // base della IDT

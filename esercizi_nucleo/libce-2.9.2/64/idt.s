#include "def.h"
.data
.balign 16
.global idt
idt:
        .space 16 * 256, 0

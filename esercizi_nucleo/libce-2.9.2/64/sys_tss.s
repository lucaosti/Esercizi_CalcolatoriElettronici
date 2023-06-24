#include "def.h"
.data
.balign 16
.global sys_tss, sys_tss_end
sys_tss:
        .space 236, 0
sys_tss_end:

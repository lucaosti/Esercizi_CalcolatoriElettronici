.text
        .global inputb
inputb:
	.cfi_startproc
        movw %di, %dx
        inb %dx, %al
        ret
	.cfi_endproc

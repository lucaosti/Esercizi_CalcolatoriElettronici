.text
        .global inputw
inputw:
	.cfi_startproc
        movw %di, %dx
        inw %dx, %ax
        ret
	.cfi_endproc

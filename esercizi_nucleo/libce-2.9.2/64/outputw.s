.text
        .global outputw
outputw:
	.cfi_startproc
        movw %di, %ax
        movw %si, %dx
        outw %ax, %dx
        ret
	.cfi_endproc

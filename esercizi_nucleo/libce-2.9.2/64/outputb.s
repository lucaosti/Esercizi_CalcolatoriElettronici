.text
        .global outputb
outputb:
	.cfi_startproc
        movb %dil, %al
        movw %si, %dx
        outb %al, %dx
        ret
	.cfi_endproc

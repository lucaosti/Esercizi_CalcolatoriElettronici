.text
        .global outputl
outputl:
	.cfi_startproc
        movl %edi, %eax
        movw %si, %dx
        outl %eax, %dx
        ret
	.cfi_endproc

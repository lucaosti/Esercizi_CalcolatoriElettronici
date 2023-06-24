.text
        .global inputl
inputl:
	.cfi_startproc
        movw %di, %dx
        inl %dx, %eax
        ret
	.cfi_endproc

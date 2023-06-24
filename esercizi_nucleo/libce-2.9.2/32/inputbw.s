// legge una sequenza di parole da una porta di I/O
	.global inputbw
inputbw:
	pushl %edi
	
	movw 8(%esp), %dx
	movl 12(%esp), %edi
	movl 16(%esp), %ecx
	cld
	rep
	insw
	
	popl %edi
	ret

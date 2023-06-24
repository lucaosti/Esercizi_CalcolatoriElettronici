// scrive una sequenza di parole in una porta di I/O
	.global outputbw
outputbw:
	pushl %esi
	
	movw 16(%esp), %dx
	movl 8(%esp), %esi
	movl 12(%esp),%ecx
	cld
	rep
	outsw
	
	popl %esi
	ret

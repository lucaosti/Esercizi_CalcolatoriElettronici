// restituisce in %eax il contenuto di cr3
	.global readCR3
readCR3:
	movl %cr3, %eax
	ret

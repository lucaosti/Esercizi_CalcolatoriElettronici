.set ICW1M, 0x20
.set ICW2M, 0x21
.set ICW3M, 0x21
.set ICW4M, 0x21
.set OCW1M, 0x21
.set OCW3M, 0x20
.set ICW1S, 0xa0
.set ICW2S, 0xa1
.set ICW3S, 0xa1
.set ICW4S, 0xa1
.set OCW1S, 0xa1
.set OCW3S, 0xa0
.set OCW2M, 0x20
.set OCW3M, 0x20
.set OCW2S, 0xa0
.set OCW3S, 0xa0

	.global disable_8259
disable_8259:
	pushl %ebp
	movl %esp, %ebp
	pushl %eax

	// master
	movb $0x11, %al		// cascata
	outb %al, $ICW1M
	movb $0x20, %al		// nuova base
	outb %al, $ICW2M
	movb $0x04, %al		// slave connesso a IR2
	outb %al, $ICW3M
	movb $0x01, %al		// modo annidato
	outb %al, $ICW4M
	inb  $OCW1M, %al	// leggiamo la maschera corrente
	movb %al, ocw1m_save	// e la salviamo per il ritorno al DOS
	movb $0b11111011, %al	// maschera tutte le interruzioni, tranne quelle
	outb %al, $OCW1M	//  provenienti dallo slave
	movb $0x48, %al
	outb %al, $OCW3M	// fully nested

	// slave
	movb $0x11, %al		// cascata
	outb %al, $ICW1S
	movb $0x28, %al		// nuova base
	outb %al, $ICW2S
	movb $0x02, %al		// id. dello slave
	outb %al, $ICW3S
	movb $0x01, %al		// modo annidato
	outb %al, $ICW4S
	inb  $OCW1S, %al	// leggiamo la maschera corrente
	movb %al, ocw1s_save	// e la salviamo per il ritorno al DOS
	movb $0b11111111, %al	// maschera tutte le interruzioni
	outb %al, $OCW1S
	movb $0x48, %al		// fully nested
	outb %al, $OCW3S

	popl %eax
	leave
	ret

// ripristina il controllore delle interruzioni, con la base compatibile con il DOS.
// Inizializzo la modalita' fully nasted e i tipi base sono rispettivamente 0x08 per il master
// e 0x70 per lo slave.
		.global enable_8259
enable_8259:
	pushl %ebp
	movl %esp, %ebp
	pushl %eax

	// master
	movb $0x11, %al		// cascata
	outb %al, $ICW1M
	movb $0x08, %al		// nuova base
	outb %al, $ICW2M
	movb $0x04, %al		// slave connesso a IR2
	outb %al, $ICW3M
	movb $0x01, %al		// modo annidato
	outb %al, $ICW4M
	movb ocw1m_save, %al	// maschera salvata in precedenza
	outb %al, $OCW1M	
	movb $0x48, %al
	outb %al, $OCW3M	// fully nested

	// slave
	movb $0x11, %al		// cascata
	outb %al, $ICW1S
	movb $0x70, %al		// nuova base
	outb %al, $ICW2S
	movb $0x02, %al		// id. dello slave
	outb %al, $ICW3S
	movb $0x01, %al		// modo annidato
	outb %al, $ICW4S
	movb ocw1s_save, %al	// maschera salvata in precedenza
	outb %al, $OCW1S
	movb $0x48, %al		// fully nested
	outb %al, $OCW3S

	popl %eax
	leave
	ret

.data
ocw1m_save:
        .byte 0
ocw1s_save:
        .byte 0

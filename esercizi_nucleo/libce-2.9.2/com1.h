namespace com1 {

	const ioaddr iRBR = 0x03F8;		// DLAB deve essere 0
	const ioaddr iTHR = 0x03F8;		// DLAB deve essere 0
	const ioaddr iLSR = 0x03FD;
	const ioaddr iLCR = 0x03FB;
	const ioaddr iDLR_LSB = 0x03F8;		// DLAB deve essere 1
	const ioaddr iDLR_MSB = 0x03F9;		// DLAB deve essere 1
	const ioaddr iIER = 0x03F9;		// DLAB deve essere 0
	const ioaddr iMCR = 0x03FC;
	const ioaddr iIIR = 0x03FA;

}

void ini_COM1();

namespace com2 {

	const ioaddr iRBR = 0x02F8;		// DLAB deve essere 0
	const ioaddr iTHR = 0x02F8;		// DLAB deve essere 0
	const ioaddr iLSR = 0x02FD;
	const ioaddr iLCR = 0x02FB;
	const ioaddr iDLR_LSB = 0x02F8;		// DLAB deve essere 1
	const ioaddr iDLR_MSB = 0x02F9;		// DLAB deve essere 1
	const ioaddr iIER = 0x02F9;		// DLAB deve essere 0
	const ioaddr iMCR = 0x02FC;
	const ioaddr iIIR = 0x02FA;

}

void ini_COM2();

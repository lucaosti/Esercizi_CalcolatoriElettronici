namespace hd {

	const ioaddr iBR  = 0x01F0;
	const ioaddr iERR = 0x01F1;
	const ioaddr iSCR = 0x01F2;
	const ioaddr iSNR = 0x01F3;
	const ioaddr iCNL = 0x01F4;
	const ioaddr iCNH = 0x01F5;
	const ioaddr iHND = 0x01F6;
	const ioaddr iCMD = 0x01F7;
	const ioaddr iSTS = 0x01F7;
	const ioaddr iDCR = 0x03F6;
	const ioaddr iASR = 0x03F6;

}

void hd_set_lba(natl lba);
void hd_wait_for_br();

namespace tim {

	static const ioaddr iCWR = 0x43;

	static const ioaddr iCTR0_LOW = 0x40;
	static const ioaddr iCTR0_HIG = 0x40;
	static const ioaddr iSTS0_LOW = 0x40;
	static const ioaddr iSTS0_HIG = 0x40;

	static const ioaddr iCTR1_LOW = 0x41;
	static const ioaddr iCTR1_HIG = 0x41;
	static const ioaddr iSTS1_LOW = 0x41;
	static const ioaddr iSTS1_HIG = 0x41;

	static const ioaddr iCTR2_LOW = 0x42;
	static const ioaddr iCTR2_HIG = 0x42;
	static const ioaddr iSTS2_LOW = 0x42;
	static const ioaddr iSTS2_HIG = 0x42;

}

void attiva_timer(natw N);

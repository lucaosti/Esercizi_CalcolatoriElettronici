namespace pci {

	const ioaddr CAP = 0x0CF8;
	const ioaddr CDP = 0x0CFC;

}

natl make_CAP(natb bus, natb dev, natb fun, natb off);

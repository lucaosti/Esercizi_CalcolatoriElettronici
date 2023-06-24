namespace piix3 {

	const natb APICBASE = 0x80;
	const natb XBCS = 0x4e;
	const natl XBCS_ENABLE = (1U << 8);
	const natw VENDOR_ID = 0x8086;
	const natw DEVICE_ID = 0x7000;

}

bool piix3_ioapic_enable(natb bus, natb dev, natb fun);
bool piix3_ioapic_disable(natb bus, natb dev, natb fun);

namespace apic {

	const natl DEST_MSK = 0xFF000000; // destination field mask
	const natl DEST_SHF = 24;         // destination field shift
	const natl MIRQ_BIT = (1U << 16); // mask irq bit
	const natl TRGM_BIT = (1U << 15); // trigger mode (1=level, 0=edge)
	const natl IPOL_BIT = (1U << 13); // interrupt polarity (0=high, 1=low)
	const natl DSTM_BIT = (1U << 11); // destination mode (0=physical, 1=logical)
	const natl DELM_MSK = 0x00000700; // delivery mode field mask (000=fixed)
	const natl DELM_SHF = 8;          // delivery mode field shift
	const natl VECT_MSK = 0x000000FF; // vector field mask
	const natl VECT_SHF = 0;          // vector field shift

	const natl RTO = 0x10;
	const natl IRQ_MAX = 24;

	extern volatile natl* EOIR;
	extern volatile natl* IOREGSEL;
	extern volatile natl* IOWIN;

}

bool apic_init();
void apic_reset();
natl apic_in(natb off);
void apic_out(natb off, natl v);
natl apic_read_rth(natb irq);
natl apic_read_rtl(natb irq);
extern "C" void apic_send_EOI();
void apic_set_DEST(natl irq, natb dest);
void apic_set_IPOL(natl irq, bool v);
void apic_set_MIRQ(natl irq, bool v);
void apic_set_TRGM(natl irq, bool v);
void apic_set_VECT(natl irq, natb vec);
void apic_write_rth(natb irq, natl w);
void apic_write_rtl(natb irq, natl w);
extern "C" natl apic_read_irr(natb i);
extern "C" natl apic_read_isr(natb i);

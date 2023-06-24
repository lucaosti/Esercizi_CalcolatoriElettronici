#include "internal.h"
#include "apic.h"
#include "piix3.h"

extern "C" void disable_8259();
bool apic_init()
{
	natb bus = 0, dev = 0, fun = 0;
        if (! pci_find_dev(bus, dev, fun, piix3::DEVICE_ID, piix3::VENDOR_ID)) {
                printf("PIIX3 non trovato\n");
                return false;
        }
        if (! piix3_ioapic_enable(bus, dev, fun)) {
		printf("IOAPIC non abilitato\n");
                return false;
	}
        disable_8259();
        return true;
}

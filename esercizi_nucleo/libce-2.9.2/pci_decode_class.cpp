#include "internal.h"

struct pci_class_codes_t {
	natb code;
	const char *des;
};

pci_class_codes_t pci_class_codes[] = {
	0x00,	"pre revision 2.0",
	0x01,	"mass storage controller",
	0x02,	"network controller",
	0x03,	"display controller",
	0x04,	"multimedia device",
	0x05,	"memory controller",
	0x06,	"bridge device",
	0x07,	"simple communication controller",
	0x08,	"base system peripheral",
	0x09,	"input device",
	0x0A,	"docking station",
	0x0B,	"processor",
	0x0C,	"serial bus controller",
	0xFF,	"other"
	};

const char* pci_decode_class(natb code)
{
	for (unsigned int i = 0; i < sizeof(pci_class_codes)/sizeof(pci_class_codes_t); i++) {
		if (pci_class_codes[i].code == code)
			return pci_class_codes[i].des;
	}
	return "unkwnown";
}

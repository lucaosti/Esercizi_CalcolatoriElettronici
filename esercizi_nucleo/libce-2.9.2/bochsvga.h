namespace bochsvga {

	extern volatile void* framebuffer;
	extern volatile natw* vgareg;
	extern volatile natw* vbeext;

	//indici per vbeext
	static const natw VBE_DISPI_INDEX_ID = 0x0;
	static const natw VBE_DISPI_INDEX_XRES = 0x1;
	static const natw VBE_DISPI_INDEX_YRES = 0x2;
	static const natw VBE_DISPI_INDEX_BPP = 0x3;
	static const natw VBE_DISPI_INDEX_ENABLE = 0x4;
	static const natw VBE_DISPI_INDEX_BANK = 0x5;
	static const natw VBE_DISPI_INDEX_VIRT_WIDTH = 0x6;
	static const natw VBE_DISPI_INDEX_VIRT_HEIGHT = 0x7;
	static const natw VBE_DISPI_INDEX_X_OFFSET = 0x8;
	static const natw VBE_DISPI_INDEX_Y_OFFSET = 0x9;

	//valori possibili per VBE_DISPI_INDEX_BPP
	static const natw VBE_DISPI_BPP_4 = 0x04;
	static const natw VBE_DISPI_BPP_8 = 0x08;
	static const natw VBE_DISPI_BPP_15 = 0x0F;
	static const natw VBE_DISPI_BPP_16 = 0x10;
	static const natw VBE_DISPI_BPP_24 = 0x18;
	static const natw VBE_DISPI_BPP_32 = 0x20;

	//bit settabili su VBE_DISPI_INDEX_ENABLE
	static const natw VBE_DISPI_DISABLED = 0x00;
	static const natw VBE_DISPI_ENABLED = 0x01;
	static const natw VBE_DISPI_GETCAPS = 0x02;
	static const natw VBE_DISPI_8BIT_DAC = 0x20;
	static const natw VBE_DISPI_LFB_ENABLED = 0x40;
	static const natw VBE_DISPI_NOCLEARMEM = 0x80;

}

//inizializza il descrittore videocard (bochsvga)
bool bochsvga_init();

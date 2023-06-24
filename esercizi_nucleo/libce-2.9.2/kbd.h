namespace kbd {

	const natl MAX_CODE = 42; 
	const ioaddr iRBR = 0x60;
	const ioaddr iTBR = 0x60;
	const ioaddr iCMR = 0x64;
	const ioaddr iSTR = 0x64;
	
	extern bool shift;
	extern natb tab[MAX_CODE];
	extern char tabmin[MAX_CODE];
	extern char tabmai[MAX_CODE];

}

void keyboard_init();
void keyboard_reset();

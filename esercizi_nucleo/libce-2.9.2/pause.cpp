#include "internal.h"
#include "kbd.h"

// attende che venga premuto il tasto ESC
void pause()
{
	str_write("Premere ESC per proseguire\n");
	while (char_read() != 0x1B)
		;
}

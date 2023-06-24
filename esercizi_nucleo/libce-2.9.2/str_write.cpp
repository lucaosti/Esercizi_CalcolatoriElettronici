#include "libce.h"

// scrive la sequenza di caratteri contenuta in buf[] 
// (lo zero finale non viene scritto)
void str_write(const char buf[])
{
	natl i = 0;
        while (buf[i]) {
		char_write(buf[i]);
		i++;
        }
}

#include "internal.h"
int puts(const char *buf)
{
        str_write(buf);
        char_write('\n');
        char_write('\r');
	return 1;
}

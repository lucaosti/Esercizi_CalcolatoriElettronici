#include "libce.h"

int int_read()
{
	int i = 0, sign = 1;
	char c;
	// skip spaces
	do {
		c = char_read();
		char_write(c);
	} while (c == ' ' || c == '\t' || c == '\n');
	// optional sign
	if (c == '-' || c == '+') {
		sign = (c == '-' ? -1 : 1);
		c = char_read();
		char_write(c);
	}
	// digits
	while (c >= '0' && c <= '9') {
		i = i * 10 + (c - '0');
		c = char_read();
		char_write(c);
	}
	return sign * i;
}

#include "internal.h"
static const char hex_map[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

void htostr(char *buf, unsigned long long l, int cifre)
{
        int i;
        for(i = cifre - 1; i >= 0; --i) {
                buf[i] = hex_map[l % 16];
                l /= 16;
        }
}

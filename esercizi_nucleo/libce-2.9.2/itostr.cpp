#include "internal.h"
void itostr(char *buf, unsigned int len, long l)
{
        unsigned int i, div = 1000000000, v, w = 0;

        if(l == (-2147483647 - 1)) {
                strncpy(buf, "-2147483648", 12);
                return;
        } else if(l < 0) {
                buf[0] = '-';
                l = -l;
                i = 1;
        } else if(l == 0) {
                buf[0] = '0';
                buf[1] = 0;
                return;
        } else
                i = 0;

        while(i < len - 1 && div != 0) {
                if((v = l / div) || w) {
                        buf[i++] = '0' + (char)v;
                        w = 1;
                }

                l %= div;
                div /= 10;
        }

        buf[i] = 0;
}

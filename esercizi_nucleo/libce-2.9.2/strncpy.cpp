#include "internal.h"
char *strncpy(char *dest, const char *src, natl l)
{
        size_t i;

        for(i = 0; i < l && src[i]; ++i)
                dest[i] = src[i];

        return dest;
}

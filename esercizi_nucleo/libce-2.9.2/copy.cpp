#include "internal.h"
char* copy(const char* src, char* dst)
{
        while (*src)
                *dst++ = *src++;
        *dst = '\0';
        return dst;
}

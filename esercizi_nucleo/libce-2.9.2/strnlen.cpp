#include "internal.h"
size_t strnlen(const char *s, size_t len)
{
        size_t l = 0;

        while(len-- && *s++)
                ++l;

        return l;
}

#include "internal.h"
void *memcpy(void *dest, const void *src, size_t n)
{
        char       *dest_ptr = static_cast<char*>(dest);
        const char *src_ptr  = static_cast<const char*>(src);

        for (size_t i = 0; i < n; i++)
                dest_ptr[i] = src_ptr[i];

        return dest;
}

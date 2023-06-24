#include "internal.h"
int printf(const char *fmt, ...)
{
        va_list ap;
        char buf[1024];
        int l;

        va_start(ap, fmt);
        l = vsnprintf(buf, 1024, fmt, ap);
        va_end(ap);

        str_write(buf);

        return l;
}

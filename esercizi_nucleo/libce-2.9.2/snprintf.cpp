#include "internal.h"
int snprintf(char *buf, natl n, const char *fmt, ...)
{
        va_list ap;
        int l;

        va_start(ap, fmt);
        l = vsnprintf(buf, n, fmt, ap);
        va_end(ap);

        return l;
}

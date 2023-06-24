#include "internal.h"
extern "C" [[noreturn]] void panic(const char *);
[[noreturn]] void fpanic(const char *fmt, ...)
{
        va_list ap;
        char buf[1024];
        int l;

        va_start(ap, fmt);
        l = vsnprintf(buf, 1024, fmt, ap);
        va_end(ap);

        panic(buf);
}

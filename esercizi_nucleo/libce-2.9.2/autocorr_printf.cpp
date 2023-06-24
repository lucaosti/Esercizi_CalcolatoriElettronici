#include "internal.h"
#define BUFSZ 1024
int autocorr_printf(const char *fmt, ...)
{
        va_list ap;
        static char buf[BUFSZ];
	static char *n = buf;
        int l, i;

        va_start(ap, fmt);
        l = vsnprintf(n, &buf[BUFSZ] - n, fmt, ap);
        va_end(ap);

	for (i = 0; i < l; i++) {
		if (n[i] == '\n') {
			n[i] = '\0';
			flog(LOG_USR, "%s", buf);
			memcpy(buf, n + i + 1, l - i - 1);
			n = buf;
			return l;
		}
	}
	n += l - 1;

        return l;
}

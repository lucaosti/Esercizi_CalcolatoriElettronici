#include "internal.h"

extern "C" void do_log(log_sev sev, const char* buf, natl size);

// log formattato
extern "C" void flog(log_sev sev, const char* fmt, ...)
{
	va_list ap;
	const natl LOG_MSG_SIZE = 256;
	char buf[LOG_MSG_SIZE];

	va_start(ap, fmt);
	int l = vsnprintf(buf, LOG_MSG_SIZE, fmt, ap);
	va_end(ap);

	if (l > 1)
		do_log(sev, buf, l);
}

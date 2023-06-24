#include "internal.h"

void panic(const char *msg)
{
	flog(LOG_ERR, "%s", msg);
	reboot();
}

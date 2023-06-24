#include "internal.h"

#ifdef __x86_64__
#define XIP "RIP"
#else
#define XIP "EIP"
#endif

extern "C" void gestore_eccezioni(unsigned long tipo, unsigned long err, void* eip)
{
        flog(LOG_WARN, "Eccezione %ld, err=%lx, " XIP "=%p", tipo,  err, eip);
}

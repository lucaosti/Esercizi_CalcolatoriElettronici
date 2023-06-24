#include "libce.h"
int pause_caller;
static void register_pause() __attribute__((constructor));
extern "C" void __cxa_atexit(void (*)(void*), void*, void*);
static void register_pause()
{
	__cxa_atexit((void(*)(void*))pause, 0, 0);
}

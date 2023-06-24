#include "internal.h"

struct exit_rec {
	void (*fun)(void *);
	void *arg;
};

static const int MAX_EXIT_FUNCS = 16;
exit_rec exit_funcs[MAX_EXIT_FUNCS];
static int next_exit_func = 0;
extern "C" void __cxa_atexit(void (*f)(void *), void *a, void *dummy)
{
	if (next_exit_func >= MAX_EXIT_FUNCS) {
		panic("too many destructors");
	}
	exit_rec *e = &exit_funcs[next_exit_func];
	e->fun = f;
	e->arg = a;
	next_exit_func++;
}

extern "C" void dtors()
{
	for (next_exit_func--; next_exit_func >= 0; next_exit_func--) {
		exit_rec *e = &exit_funcs[next_exit_func];
		e->fun(e->arg);
	}
}

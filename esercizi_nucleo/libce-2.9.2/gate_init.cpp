#include "internal.h"

struct gate {
	natl dw1;
	natl dw2;
#ifdef __x86_64__
	natl dw3;
	natl dw4;
#endif
};

extern gate idt[];
extern "C" void componi_gate(gate& g, void routine(), bool trap);

void gate_init(natl num, void routine())
{
	gate gg;

	componi_gate(gg, routine, false /* interrupt */);
	idt[num] = gg;
}

void trap_init(natl num, void routine())
{
	gate gg;

	componi_gate(gg, routine, true /* trap */);
	idt[num] = gg;
}

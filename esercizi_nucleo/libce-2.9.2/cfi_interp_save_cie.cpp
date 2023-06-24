#include "tipo.h"
#include "cfi.h"
#include "cfi_internal.h"

void cfi_interp::save_cie()
{
	for (int i = 0; i < CFI::NREG; i++)
		cie_regs[i] = regs[i];
}

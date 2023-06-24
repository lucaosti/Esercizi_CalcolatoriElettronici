#include "tipo.h"
#include "cfi.h"
#include "cfi_internal.h"

void cfi_interp::snapshot()
{
	natq bregs[CFI::NREG];

	cfa = cfi->regs[cfa_register] + cfa_offset;
	for (int i = 0; i < CFI::NREG; i++) {
		switch (regs[i].mode) {
		case cfi_mode::UNDEFINED:
			break;
		case cfi_mode::SAME_VALUE:
			bregs[i] = cfi->regs[i];
			break;
		case cfi_mode::OFFSET:
			bregs[i] = cfi->read_stack(cfi->token, cfa + regs[i].offset);
			break;
		case cfi_mode::VAL_OFFSET:
			bregs[i] = cfa + regs[i].offset;
			break;
		case cfi_mode::REGISTER:
			bregs[i] = cfi->regs[regs[i].regn];
			break;
		case cfi_mode::EXPRESSION:
			// TODO
			break;
		case cfi_mode::VAL_EXPRESSION:
			// TODO
			break;
		}
	}

	for (int i = 0; i < CFI::NREG; i++) {
		if (i == CFI::RSP && regs[i].mode == cfi_mode::UNDEFINED) {
			cfi->regs[i] = cfa;
		} else {
			cfi->regs[i] = bregs[i];
		}
	}
}

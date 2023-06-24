#include "tipo.h"
#include "cfi.h"
#include "cfi_internal.h"

void cfi_interp::init(cfi_d* cfi_, vaddr rip_ini)
{
	cfi = cfi_;
	for (cfi_reg_info& r: regs) {
		r.mode = cfi_mode::UNDEFINED;
		r.offset = 0;
		r.regn = 0;
	}
	regs[CFI::RBP].mode = cfi_mode::SAME_VALUE;
	regs[CFI::RBX].mode = cfi_mode::SAME_VALUE;
	regs[CFI::R10].mode = cfi_mode::SAME_VALUE;
	regs[CFI::R11].mode = cfi_mode::SAME_VALUE;
	regs[CFI::R12].mode = cfi_mode::SAME_VALUE;
	regs[CFI::R13].mode = cfi_mode::SAME_VALUE;
	regs[CFI::R14].mode = cfi_mode::SAME_VALUE;
	regs[CFI::R15].mode = cfi_mode::SAME_VALUE;

	next_op = 0;
	cur_loc = rip_ini;
}

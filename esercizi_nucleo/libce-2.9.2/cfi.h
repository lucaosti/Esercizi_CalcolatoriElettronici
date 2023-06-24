enum CFI {
	RAX = 0,
	RDX,
	RCX,
	RBX,
	RSI,
	RDI,
	RBP,
	RSP,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15,
	RA,	// return address

	NREG
};


struct cfi_d {
	natq regs[CFI::NREG];
	vaddr eh_frame;
	natq eh_frame_len;
	void *token;
	natq (*read_stack)(void* token, vaddr);
};

bool cfi_backstep(cfi_d& cfi, vaddr rip);


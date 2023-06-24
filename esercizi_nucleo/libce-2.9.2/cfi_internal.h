enum cfi_mode {
	UNDEFINED,
	SAME_VALUE,
	OFFSET,
	VAL_OFFSET,
	REGISTER,
	EXPRESSION,
	VAL_EXPRESSION
};

struct cfi_reg_info {
	cfi_mode mode;
	long offset;
	int regn;
};

struct cfi_interp {
	cfi_d *cfi;
	cfi_reg_info regs[CFI::NREG];
	cfi_reg_info cie_regs[CFI::NREG];
	vaddr cfa;
	long cfa_offset;
	int cfa_register;
	natq caf;
	long daf;
	int rar;

	vaddr next_op;
	vaddr cur_loc;

	void init(cfi_d*, vaddr);
	void save_cie();
	void jump(vaddr);
	void exec();
	void snapshot();
};

natq read_uleb128(vaddr& scan);
long read_sleb128(vaddr& scan);

#include "def.h"
.global comm_exc
comm_exc:
	.cfi_startproc
	call gestore_eccezioni
	hlt
	.cfi_endproc


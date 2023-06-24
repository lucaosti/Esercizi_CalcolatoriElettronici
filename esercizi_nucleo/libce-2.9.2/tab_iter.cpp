#include "internal.h"
#include "vm.h"

tab_iter::tab_iter(paddr tab, vaddr beg, natq dim, int lvl)
{
	// situazioni di errore:
	// - lvl non valido
	// - wrap-around
	// - attraversamento del "buco" di indirizzi
	// Le ultime due condizioni si possono controllare verificando che beg
	// e beg+dim-1 (ultimo byte dell'intervallo) abbiano lo stesso bit di segno
	if (lvl < 1 || lvl > MAX_LIV || !valid_interval(beg, dim)) {
		panic("tab_iter: parametri non validi");
		return;
	}

	if (dim == 0) {
		// ci portiamo in una situazione di visita terminata
		l = MAX_LIV + 1;
		sp(l)->tab = 0;
		return;
	}

	vaddr end = beg + dim;

	pp()->cur = beg;
	pp()->end = end;
	pp()->tab = 0;

	l = lvl;
	sp(l + 1)->tab = 0;
	sp()->tab = tab;
	sp()->cur = base(beg, l - 1);
	sp()->end = limit(end, l - 1);
}

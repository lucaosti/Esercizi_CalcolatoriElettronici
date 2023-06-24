#include "libce.h"
#include "vm.h"


void tab_iter::next()
{
	if (done())
		return;

	// prima proviamo a scendere
	if (down())
		return;

	// altrimenti cerchiamo il primo livello dal basso in cui si 
	// possa andare a destra, partendo da quello corrente
	do {
		if (right())
			return;
	} while (up());
	// se arriviamo qui, la visita è finita
}

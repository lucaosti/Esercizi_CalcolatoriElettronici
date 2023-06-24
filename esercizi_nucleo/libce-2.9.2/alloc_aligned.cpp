#include "libce.h"
#include "heap.h"

void *alloc_aligned(size_t dim, align_val_t a)
{
	natq alignment = *reinterpret_cast<natq*>(&a);

	if (!alignment || (alignment & (alignment - 1)))
		return nullptr;

	dim = allinea(dim, sizeof(des_mem));

	size_t newdim = dim + alignment + sizeof(des_mem);
	if (newdim < dim) // overflow
		return nullptr;

	void *p = alloca(newdim);
	if (p == nullptr)
		return p;

	// calcoliamo il primo indirizzo successivo a p e allineato ad
	// 'alignment'
	natq m1 = reinterpret_cast<natq>(p);
	natq m2 = (m1 + alignment - 1) & ~(alignment - 1);
	des_mem *d = reinterpret_cast<des_mem*>(p) - 1;
	if (m2 != m1) {
		// dobbiamo deallocare la prima parte e spostare p
		size_t allocdim = d->dimensione,
		       skipdim = m2 - m1 - sizeof(des_mem);
		d->dimensione = skipdim;
		dealloca(p);
		p = reinterpret_cast<void*>(m2);
		d = reinterpret_cast<des_mem*>(p) - 1;
		d->dimensione = allocdim - skipdim - sizeof(des_mem);
		d->next = reinterpret_cast<des_mem*>(0xdeadbeef);
	}
	if (d->dimensione - dim >= 2 * sizeof(des_mem)) {
		free_interna(reinterpret_cast<void*>(m2 + dim), d->dimensione - dim);
		d->dimensione = dim;
	}
	return p;
}

#include "tipo.h"
#include "cfi.h"
#include "cfi_internal.h"

bool cfi_backstep(cfi_d& cfi, vaddr rip)
{
	vaddr cie_ops = 0, cie_end;
	vaddr fde_ops = 0, fde_range_beg, fde_end;
	bool has_z = false;
	cfi_interp is;

	// primo passo: trovare  l'FDE che contiene rip
	vaddr scan = cfi.eh_frame;
	while (scan - cfi.eh_frame < cfi.eh_frame_len) {
		natl length = *reinterpret_cast<natl*>(scan);
		scan += 4;
		natl id = *reinterpret_cast<natl*>(scan);
		if (!id) {
			// CIE
			vaddr cie_scan = scan + 5;
			cie_end = scan + length;
			const char* as = reinterpret_cast<const char*>(cie_scan);
			for (int i = 0; as[i]; i++)
				cie_scan++;
			cie_scan++; // saltiamo il byte nullo che termina la stringa
			is.caf = read_uleb128(cie_scan);
			is.daf = read_sleb128(cie_scan);
			is.rar = read_uleb128(cie_scan);
			vaddr as_scan = cie_scan;
			vaddr as_size = 0;
			for ( ; *as; as++) {
				switch (*as) {
				case 'z':
					has_z = true;
					as_size = read_uleb128(as_scan);
					cie_scan = as_scan;
					break;
				case 'R':
					if (*reinterpret_cast<natb*>(as_scan) != 0x1b)
						return false;
					as_scan++;
					break;
				default:
					// not supported
					return false;
				}
			}
			cie_ops = cie_scan + as_size;
		} else {
			// FDE
			vaddr fde_scan = scan + 4;
			fde_range_beg = fde_scan + *reinterpret_cast<int*>(fde_scan);
			fde_scan += 4;
			vaddr len = *reinterpret_cast<int*>(fde_scan);
			fde_scan += 4;
			if (rip >= fde_range_beg && rip < fde_range_beg + len) {
				if (has_z) {
					vaddr as_size = read_uleb128(fde_scan);
					fde_scan += as_size;
				}
				fde_ops = fde_scan;
				fde_end = scan + length;
				break;
			}
		}
		scan += length;
	}
	if (!fde_ops)
		return false;

	// ora interprettiamo le istruzioni, prima del cie poi dell'fde
	is.init(&cfi, fde_range_beg);

	if (cie_ops) {
		is.jump(cie_ops);
		while (is.next_op < cie_end)
			is.exec();
	}
	is.save_cie();

	is.jump(fde_ops);
	while (is.next_op < fde_end && is.cur_loc <= rip)
		is.exec();

	// ora calcoliamo lo stato precedente
	is.snapshot();
	return true;
}

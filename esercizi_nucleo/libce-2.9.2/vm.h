static const int MAX_LIV = 4;
static const natq BIT_SEGNO = (1ULL << (12 + 9 * MAX_LIV - 1));
static const natq MASCHERA_MODULO = BIT_SEGNO - 1;
// restituisce la versione normalizzata (16 bit piu' significativi uguali al
// bit 47) dell'indirizzo a
static inline constexpr vaddr norm(vaddr a)
{
	return (a & BIT_SEGNO) ? (a | ~MASCHERA_MODULO) : (a & MASCHERA_MODULO);
}

// restituisce la dimensione di una regione di livello liv
static inline constexpr natq dim_region(int liv)
{
	natq v = 1ULL << (liv * 9 + 12);
	return v;
}

// dato un indirizzo 'v', restituisce l'indirizzo della
// regione di livello 'liv' a cui 'v' appartiene
static inline constexpr vaddr base(vaddr v, int liv)
{
	natq mask = dim_region(liv) - 1;
	return v & ~mask;
}

// dato l'idirizzo 'e', estremo destro di un intervallo [b, e),
// restituisce l'indirizzo della prima regione di livello 'liv'
// a destra dell'intervallo.
static inline constexpr vaddr limit(vaddr e, int liv)
{
	natq dr = dim_region(liv);
	natq mask = dr - 1;
	return (e + dr - 1) & ~mask;
}
using tab_entry = natq;

//   ( definiamo alcune costanti utili per la manipolazione dei descrittori
//     di pagina e di tabella. Assegneremo a tali descrittori il tipo "natq"
//     e li manipoleremo tramite maschere e operazioni sui bit.
const natq BIT_P    = 1U << 0; // il bit di presenza
const natq BIT_RW   = 1U << 1; // il bit di lettura/scrittura
const natq BIT_US   = 1U << 2; // il bit utente/sistema
const natq BIT_PWT  = 1U << 3; // il bit Page Wright Through
const natq BIT_PCD  = 1U << 4; // il bit Page Cache Disable
const natq BIT_A    = 1U << 5; // il bit di accesso
const natq BIT_D    = 1U << 6; // il bit "dirty"
const natq BIT_PS   = 1U << 7; // il bit "page size"

const natq ACCB_MASK  = 0x00000000000000FF; // maschera per il byte di accesso
const natq ADDR_MASK  = 0x7FFFFFFFFFFFF000; // maschera per l'indirizzo

// )

static inline constexpr paddr extr_IND_FISICO(tab_entry descrittore)
{ // (
	return descrittore & ADDR_MASK; // )
}
static inline void  set_IND_FISICO(tab_entry& descrittore, paddr ind_fisico) //
{ // (
	descrittore &= ~ADDR_MASK;
	descrittore |= ind_fisico & ADDR_MASK; // )
}

// dato un indirizzo virtuale 'ind_virt' ne restituisce
// l'indice del descrittore corrispondente nella tabella di livello 'liv'
static inline constexpr int i_tab(vaddr ind_virt, int liv)
{
	int shift = 12 + (liv - 1) * 9;
	natq mask = 0x1ffULL << shift;
	return (ind_virt & mask) >> shift;
}
// dato l'indirizzo di una tabella e un indice, restituisce un
// riferimento alla corrispondente entrata
static inline tab_entry& get_entry(paddr tab, natl index)
{
	tab_entry *pd = reinterpret_cast<tab_entry*>(tab);
	return  pd[index];
}

class tab_iter {

	// ogni riga dello stack, partendo dalla 0, traccia la posizione
	// corrente all'interno dell'albero di traduzione.  La riga 0 traccia
	// la posizione nell'albero al livello massimo, la riga 1 nel livello
	// subito inferiore, e così via. Il livello corrente è contenuto
	// nella variabile 'l'.
	//
	// Ogni riga dello stack contiene l'indirizzo fisico (tab) della
	// tabella, del livello corrispondente, attualmente in corso di visita.
	// La riga contiene anche un intervallo [cur, end) di indirizzi ancora
	// da visitare in quella tabella. I campi cur e end della riga MAX_LIV,
	// altrimenti inutilizzati, vengono usati per contenere gli estremi
	// dell'intervallo completo passato al costruttore dell'iteratore.
	//
	// La terminazione della visita si riconosce da p->tab == 0.

	struct stack {
		vaddr cur, end;
		paddr tab;
	} s[MAX_LIV + 1];

	int l;

	stack *sp() { return &s[l - 1]; }
	stack const *sp() const { return &s[l - 1]; }
	stack *sp(int lvl) { return &s[lvl - 1]; }
	stack *pp() { return &s[MAX_LIV]; }
	stack const *pp() const { return &s[MAX_LIV]; }
	bool done() const { return !sp()->tab; }

public:
	static bool valid_interval(vaddr beg, natq dim)
	{
		vaddr end = beg + dim - 1;
		return !dim || (
			// non inizia nel buco
			     norm(beg) == beg
			// non termina nel buco
			&&   norm(end) == end
			// tutto dalla stessa parte rispetto al buco
			&&   (beg & BIT_SEGNO) == (end & BIT_SEGNO)
			// non fa wrap-around
			&&   end >= beg);
	}

	tab_iter(paddr tab, vaddr beg, natq dim = 1, int liv = MAX_LIV);

	// conversione a booleano: true sse la visita è terminata
	// (le altre funzioni non vanno chiamate se l'iteratore è in questo stato)
	operator bool() const {
		return !done();
	}

	// restituisce il livello su cui si trova l'iteratore
	int get_l() const {
		return l;
	}

	// restituisce true sse l'iteratore si trova su una foglia
	bool is_leaf() const {
		tab_entry e = get_e();
		return !(e & BIT_P) || (e & BIT_PS) || l == 1;
	}

	// restituisce il più piccolo indirizzo virtuale appartenente
	// all'intevallo e la cui traduzione passa dal descrittore corrente
	vaddr get_v() const {
		return max(pp()->cur, sp()->cur);
	}

	// restituisce un riferimento al descrittore corrente
	tab_entry& get_e() const {
		return get_entry(sp()->tab, i_tab(sp()->cur, l));
	}

	// restituisce l'indirizzo fisico della tabella che contiene il
	// descrittore corrente
	paddr get_tab() const {
		return sp()->tab;
	}

	// prova a spostare l'iteratore di una posizione in basso nell'albero,
	// se possibile, altrimenti non fa niente. Restituisce true sse lo
	// spostamento è avvenuto.
	bool down();
	// prova a spostare l'iteratore di una posizione in alto nell'albero,
	// se possibile, altrimenti non fa niente. Restituisce true sse lo
	// spostamento è avvenuto.
	bool up();
	// prova a spostare l'iteratore di una posizione a destra nell'albero
	// (rimanendo nel livello corrente), se possibile, altrimenti non fa
	// niente. Restituisce true sse lo spostamento è avvenuto.
	bool right();

	// porta l'iteratore alla prossima posizione della visita in ordine
	// anticipato
	void next();

	// inizia una visita in ordine posticipato
	void post();
	// porta l'iteratore alla prossima posizione della visita in ordine
	// posticipato
	void next_post();
};
// carica un nuovo valore in cr3
extern "C" void loadCR3(paddr dir);

// restituisce il valore corrente di cr3
extern "C" paddr readCR3();

//invalida il TLB
extern "C" void invalida_TLB();

// invalida una entrata del TLB
extern "C" void invalida_entrata_TLB(vaddr v);

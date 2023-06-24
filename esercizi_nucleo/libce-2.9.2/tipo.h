using ioaddr = unsigned short;
using natb   = unsigned char;
using natw   = unsigned short;
using natl   = unsigned int;
#if defined(__x86_64__) || defined(CE_UTILS)
using size_t = unsigned long;
using ssize_t = long;
using ptrdiff_t = long;
using intmax_t = long;
using uintmax_t = unsigned long;
using uptrdiff_t = unsigned long;
using uintptr_t = unsigned long;
using natq   = unsigned long;
using vaddr  = unsigned long;
using paddr  = unsigned long;
#else
using size_t = unsigned int;
using ssize_t = int;
using ptrdiff_t = int;
using intmax_t = int;
using uintmax_t = unsigned int;
using uptrdiff_t = unsigned int;
using uintptr_t = unsigned int;
using natq   = unsigned long long;
using vaddr  = unsigned long long;
using paddr  = unsigned long long;
#endif

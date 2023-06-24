#include "tipo.h"

namespace apic {

volatile natl* EOIR = reinterpret_cast<volatile natl*>(0xFEE000B0);

}

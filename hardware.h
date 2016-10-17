#ifndef __HARDWARE__
#define __HARDWARE__

#include "type.h"

const int SIZE = 1e5 + 10;
const int len_filename = 32;
const int num_register = 32;

extern uc mem[SIZE << 2];       /* memory */
extern ull reg[num_register];   /* reigster */
extern ull pc;                  /* PC register */

#endif  // __HARDWARE__

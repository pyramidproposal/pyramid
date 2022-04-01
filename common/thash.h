#ifndef PYR_THASH_H
#define PYR_THASH_H

#include <stdint.h>

void thash(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, const uint32_t addr[8]);

#endif

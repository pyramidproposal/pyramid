#ifndef PYR_THASH_H
#define PYR_THASH_H

#include <stdint.h>

void thash(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, const uint32_t addr[8]);

/*
  Jasmin testing export function definitions.
*/
void thash1_jazz(unsigned char *out, /*const unsigned char *in,*/
                 const unsigned char *pub_seed, const uint32_t addr[8]);

void thash2_jazz(unsigned char *out, /*const unsigned char *in,*/
                 const unsigned char *pub_seed, const uint32_t addr[8]);

void thashLEN_jazz(unsigned char *out, const unsigned char *in,
		   const unsigned char *pub_seed, const uint32_t addr[8]);

#endif

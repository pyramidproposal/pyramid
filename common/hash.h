#ifndef PYR_HASH_H
#define PYR_HASH_H

#include <stddef.h>
#include <stdint.h>

void initialize_hash_function(const unsigned char *pk_seed,
                              const unsigned char *sk_seed);

void prf(unsigned char *out, const unsigned char *key,
         const unsigned char *pub_seed, const uint32_t addr[8]);

/*
 * One may consider "uint64_t index", instead of addr,
 * if it is more convenient for the caller.
 */
void h_msg(unsigned char *out, const unsigned char *R,
           const unsigned char *root, const uint32_t addr[8],
           const unsigned char *m, size_t mlen);

void fsg(unsigned char *out, unsigned char *next_seed, const unsigned char *in,
         const unsigned char *pub_seed, const uint32_t addr[8]);

#endif

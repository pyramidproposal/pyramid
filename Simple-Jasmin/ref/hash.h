#ifndef PYR_HASH_H
#define PYR_HASH_H

#include <stddef.h>
#include <stdint.h>

void prf(unsigned char *out, const unsigned char *key,
         const unsigned char *pub_seed, const uint32_t addr[8]);

void h_msg(unsigned char *out, const unsigned char *R,
           const unsigned char *root, const uint32_t addr[8],
           const unsigned char *m, size_t mlen);

void fsg(unsigned char *out, unsigned char *next_seed, const unsigned char *in,
         const unsigned char *pub_seed, const uint32_t addr[8]);

/*
  Jasmin testing export function definitions.
*/

void prf_jazz(unsigned char *out, const unsigned char *key,
              const unsigned char *pub_seed, const uint32_t addr[8]);

void h_msg_jazz(unsigned char *out, const unsigned char *R,
                const unsigned char *root, const uint32_t addr[8],
                const unsigned char *m, size_t mlen);

void fsg_jazz(unsigned char *out, unsigned char *next_seed,
              const unsigned char *in, const unsigned char *pk,
              const uint32_t addr[8]);

void fsg_0_jazz(unsigned char *out, const unsigned char *in,
                const unsigned char *pk, const uint32_t addr[8]);

void fsg_1_jazz(unsigned char *next_seed, const unsigned char *in,
                const unsigned char *pk, const uint32_t addr[8]);

#endif

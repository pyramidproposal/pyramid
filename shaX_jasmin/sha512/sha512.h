#ifndef SHA_512_H
#define SHA_512_H

#include <stdint.h>

/* #define INLEN 3 */
#define INLEN 112

void iterate_hash_jazz(const uint64_t M[16], uint64_t H[8]);

void sha512_INLEN_jazz(unsigned char output[64],
                       const unsigned char input[INLEN]);

uint64_t get_INLEN_jazz();

int crypto_hashblocks_jazz(unsigned char *statebytes, const unsigned char *in,
                           unsigned long long inlen);

int crypto_hash_jazz(unsigned char *out, const unsigned char *in,
                     unsigned long long inlen);

#endif

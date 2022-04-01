#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>

/* #define INLEN 3 */
#define INLEN 56

void iterate_hash_jazz(const uint32_t M[16], uint32_t H[8]);

void sha256_INLEN_jazz(unsigned char output[32],
                       const unsigned char input[INLEN]);

uint64_t get_INLEN_jazz();

int crypto_hashblocks_jazz(unsigned char *statebytes, const unsigned char *in,
                           unsigned long long inlen);

int crypto_hash_jazz(unsigned char *out, const unsigned char *in,
                     unsigned long long inlen);

#endif

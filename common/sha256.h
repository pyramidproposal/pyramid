#ifndef PYR_SHA256_H
#define PYR_SHA256_H

#define PYR_SHA256_BLOCK_BYTES 64
#define PYR_SHA256_OUTPUT_BYTES 32  /* This does not necessarily equal PYR_N */

#define PYR_SHA512_BLOCK_BYTES 128
#define PYR_SHA512_OUTPUT_BYTES 64

#if PYR_SHA256_OUTPUT_BYTES < PYR_N
    #error Linking against SHA-256 with N larger than 32 bytes is not supported
#endif

#define PYR_SHA256_ADDR_BYTES 22

#include <stddef.h>
#include <stdint.h>

void sha256_inc_init(uint8_t *state);
void sha256_inc_blocks(uint8_t *state, const uint8_t *in, size_t inblocks);
void sha256_inc_finalize(uint8_t *out, uint8_t *state, const uint8_t *in, size_t inlen);
void sha256(uint8_t *out, const uint8_t *in, size_t inlen);

void sha512_inc_init(uint8_t *state);
void sha512_inc_blocks(uint8_t *state, const uint8_t *in, size_t inblocks);
void sha512_inc_finalize(uint8_t *out, uint8_t *state, const uint8_t *in, size_t inlen);
void sha512(uint8_t *out, const uint8_t *in, size_t inlen);

void mgf1_256(unsigned char *out, unsigned long outlen,
          const unsigned char *in, unsigned long inlen);

void mgf1_512(unsigned char *out, unsigned long outlen,
          const unsigned char *in, unsigned long inlen);

extern uint8_t state_seeded[40];

void seed_state(const unsigned char *pub_seed);


#endif

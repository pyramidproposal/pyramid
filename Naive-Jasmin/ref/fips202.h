#ifndef SPX_FIPS202_H
#define SPX_FIPS202_H

#include <stddef.h>
#include <stdint.h>

#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHA3_256_RATE 136
#define SHA3_512_RATE 72

void shake128_absorb(uint64_t *s, const uint8_t *input, size_t inlen);

void shake128_squeezeblocks(uint8_t *output, size_t nblocks, uint64_t *s);

void shake128_inc_init(uint64_t *s_inc);
void shake128_inc_absorb(uint64_t *s_inc, const uint8_t *input, size_t inlen);
void shake128_inc_finalize(uint64_t *s_inc);
void shake128_inc_squeeze(uint8_t *output, size_t outlen, uint64_t *s_inc);

void shake256_absorb(uint64_t *s, const uint8_t *input, size_t inlen);
void shake256_squeezeblocks(uint8_t *output, size_t nblocks, uint64_t *s);

void shake256_inc_init(uint64_t *s_inc);
void shake256_inc_absorb(uint64_t *s_inc, const uint8_t *input, size_t inlen);
void shake256_inc_finalize(uint64_t *s_inc);
void shake256_inc_squeeze(uint8_t *output, size_t outlen, uint64_t *s_inc);

void shake128(uint8_t *output, size_t outlen,
              const uint8_t *input, size_t inlen);

void shake256(uint8_t *output, size_t outlen,
              const uint8_t *input, size_t inlen);

void sha3_256_inc_init(uint64_t *s_inc);
void sha3_256_inc_absorb(uint64_t *s_inc, const uint8_t *input, size_t inlen);
void sha3_256_inc_finalize(uint8_t *output, uint64_t *s_inc);

void sha3_256(uint8_t *output, const uint8_t *input, size_t inlen);

void sha3_512_inc_init(uint64_t *s_inc);
void sha3_512_inc_absorb(uint64_t *s_inc, const uint8_t *input, size_t inlen);
void sha3_512_inc_finalize(uint8_t *output, uint64_t *s_inc);

void sha3_512(uint8_t *output, const uint8_t *input, size_t inlen);

/* 
   The following functions have their static modifier removed for tests.
*/
uint64_t load64(const unsigned char *x);
void store64(uint8_t *x, uint64_t u);
void KeccakF1600_StatePermute(uint64_t * state);
void keccak_absorb(uint64_t *s, uint32_t r, const uint8_t *m, size_t mlen, uint8_t p);
void keccak_squeezeblocks(uint8_t *h, size_t nblocks, uint64_t *s, uint32_t r);


/*
  Jasmin testing export function definitions.
*/

/* 1. Keccak components */
uint64_t load64_jazz(const unsigned char *x);
void store64_jazz(uint8_t *x, uint64_t u);
void KeccakF1600_StatePermute_jazz(uint64_t *state);
void keccak_squeezeblocks_256_256_jazz(unsigned char *h, uint64_t *s);

/* 2. shake256: absorb X_INLEN bytes, squeeze PYR_N bytes. */

/* hash_shake256 */
void shake256_PYR_N_PRF_INLEN_jazz(unsigned char *output,
                                   const unsigned char *input);
void shake256_PYR_N_PRF_KEYGEN_INLEN_jazz(unsigned char *output,
					  const unsigned char *input);

/* thash_shake256 (simple & robust) */
void shake256_PYR_N_TH1_INLEN_jazz(unsigned char *output,
                                   const unsigned char *input);
void shake256_PYR_N_TH2_INLEN_jazz(unsigned char *output,
                                   const unsigned char *input);
void shake256_PYR_N_THLEN_INLEN_jazz(unsigned char *output,
                                     const unsigned char *input);

/* thash_shake256 (robust) */
void shake256_TH1_MASK_INLEN_jazz(unsigned char *output,
                                  const unsigned char *input);
void shake256_TH2_MASK_INLEN_jazz(unsigned char *output,
                                  const unsigned char *input);
void shake256_THLEN_MASK_INLEN_jazz(unsigned char *output,
				    const unsigned char *input);

/* 3. Absorb X_INLEN bytes at a rate of SHAKE256. */

/* hash_shake256 */
void keccak_absorb_256_PRF_INLEN_jazz(uint64_t *s, const unsigned char *m);
void keccak_absorb_256_PRF_KEYGEN_INLEN_jazz(uint64_t *s, const unsigned char *m);

/* thash_shake256 (simple & robust) */
void keccak_absorb_256_TH1_INLEN_jazz(uint64_t *s, const unsigned char *m);
void keccak_absorb_256_TH2_INLEN_jazz(uint64_t *s, const unsigned char *m);
void keccak_absorb_256_THLEN_INLEN_jazz(uint64_t *s, const unsigned char *m);

/* thash_shake256 (robust) */
void keccak_absorb_256_MASK_INLEN_jazz(uint64_t *s, const unsigned char *m);

/* 4. Absorb X_INLEN bytes at a rate of SHAKE256. */

/* hash_shake256, thash_shake256 (simple & robust) */
void keccak_squeezeblocks_256_PYR_N_jazz(unsigned char *h, uint64_t *s);

/* thash_shake256 (robust) */
void keccak_squeezeblocks_256_TH1_jazz(unsigned char *h, uint64_t *s);
void keccak_squeezeblocks_256_TH2_jazz(unsigned char *h, uint64_t *s);
void keccak_squeezeblocks_256_THLEN_jazz(unsigned char *h, uint64_t *s);    

/* 5. Variable length (incremental). */
void keccak_inc_init_jazz(uint64_t *s_inc);
void keccak_inc_absorb_256_HMSG_CONST_INLEN_jazz(uint64_t *s_inc, const uint8_t *m);
void keccak_inc_absorb_256_jazz(uint64_t *s_inc, const uint8_t *m, size_t mlen);
void keccak_inc_finalize_256_jazz(uint64_t *s_inc);
void keccak_inc_squeeze_256_PYR_N_jazz(uint8_t *h, uint64_t *s);

#endif

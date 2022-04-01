#ifndef PYR_API_H
#define PYR_API_H

#include <stddef.h>
#include <stdint.h>

#include "ext_algo.h"
#include "format.h"
#include "params.h"

#define CRYPTO_ALGNAME "Pyramid"

#define CRYPTO_SECRETKEYBYTES (PYR_SK_CORE_BYTES + SK_EXT_ALGO_BYTES)
#define CRYPTO_PUBLICKEYBYTES PYR_PK_BYTES
#define CRYPTO_BYTES PYR_BYTES
#define CRYPTO_SEEDBYTES ((2 + NR_SSD) * PYR_N)
#define CRYPTO_MAX_VALID_IDX                                  \
  ((LAST_VALID_IDX < IDX_EXT_ALGO_MAX_VALID) ? LAST_VALID_IDX \
                                             : IDX_EXT_ALGO_MAX_VALID)

/**
 * Returns the length of a secret key, in bytes
 */
unsigned long long crypto_sign_secretkeybytes(void);

/**
 * Returns the length of a public key, in bytes
 */
unsigned long long crypto_sign_publickeybytes(void);

/**
 * Returns the length of a signature, in bytes
 */
unsigned long long crypto_sign_bytes(void);

/**
 * Returns the length of the seed required to generate a key pair, in bytes
 */
unsigned long long crypto_sign_seedbytes(void);

/**
 * Generates a Pyramid key pair given a seed.
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [root || PUB_SEED]
 */
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed);

/**
 * Generates a Pyramid key pair.
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [root || PUB_SEED]
 */
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen, const uint8_t *m,
                          size_t mlen, uint8_t *sk);

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen, const uint8_t *m,
                       size_t mlen, const uint8_t *pk);

/**
 * Returns an array containing the signature followed by the message.
 */
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                unsigned char *sk);

/**
 * Verifies a given signature-message pair under a given public key.
 */
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);

#endif

#include "api.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "export.h"
#include "format.h"
#include "randombytes.h"

/*
 * Returns the length of a secret key, in bytes
 */
unsigned long long crypto_sign_secretkeybytes(void) {
  return CRYPTO_SECRETKEYBYTES;
}

/*
 * Returns the length of a public key, in bytes
 */
unsigned long long crypto_sign_publickeybytes(void) {
  return CRYPTO_PUBLICKEYBYTES;
}

/*
 * Returns the length of a signature, in bytes
 */
unsigned long long crypto_sign_bytes(void) { return CRYPTO_BYTES; }

/*
 * Returns the length of the seed required to generate a key pair, in bytes
 */
unsigned long long crypto_sign_seedbytes(void) { return CRYPTO_SEEDBYTES; }

/*
 * Generates an PYR key pair given a seed.
 */
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed) {
  return crypto_sign_seed_keypair_jazz(pk, sk, seed);
}

/*
 * Generates an PYR key pair.
 */
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk) {
  unsigned char seed[CRYPTO_SEEDBYTES];
  randombytes(seed, CRYPTO_SEEDBYTES);
  return crypto_sign_seed_keypair(pk, sk, seed);
}

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen, const uint8_t *m,
                          size_t mlen, uint8_t *sk) {
  return crypto_sign_signature_jazz(sig, siglen, m, mlen, sk);
}

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen, const uint8_t *m,
                       size_t mlen, const uint8_t *pk) {
  return crypto_sign_verify_jazz(sig, siglen, m, mlen, pk);
}

/**
 * Returns an array containing the signature followed by the message.
 */
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                unsigned char *sk) {
  size_t siglen;
  int retval;

  retval = crypto_sign_signature(sm, &siglen, m, (size_t)mlen, sk);

  memmove(sm + PYR_BYTES, m, mlen);
  *smlen = siglen + mlen;

  return retval;
}

/**
 * Verifies a given signature-message pair under a given public key.
 */
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk) {
  /* The API caller does not necessarily know what size a signature should be
     but Pyramid signatures are always exactly PYR_BYTES. */

  int tmp;
  if (smlen < PYR_BYTES) {
    memset(m, 0, smlen);
    *mlen = 0;
    return -1;
  }

  *mlen = smlen - PYR_BYTES;

  if ((tmp = crypto_sign_verify(sm, PYR_BYTES, sm + PYR_BYTES, (size_t)*mlen,
                                pk))) {
    memset(m, 0, smlen);
    *mlen = 0;
    return -1;
  }

  /* If verification was successful, move the message to the right place. */
  memmove(m, sm + PYR_BYTES, *mlen);

  return 0;
}

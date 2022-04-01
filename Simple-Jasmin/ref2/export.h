#ifndef PYR_EXPORT_H
#define PYR_EXPORT_H

#include <stddef.h>
#include <stdint.h>

/*
   It is (currently) not possible to define an export- and an inline/noinline fn
   with the same name. Therefore, the suffix "_jazz" is used for the export fn.
*/

int crypto_sign_seed_keypair_jazz(unsigned char *pk, unsigned char *sk,
                                  const unsigned char *seed);

int crypto_sign_signature_jazz(uint8_t *sig, size_t *siglen, const uint8_t *m,
                               size_t mlen, uint8_t *sk);

int crypto_sign_verify_jazz(const uint8_t *sig, size_t siglen, const uint8_t *m,
                            size_t mlen, const uint8_t *pk);

#endif

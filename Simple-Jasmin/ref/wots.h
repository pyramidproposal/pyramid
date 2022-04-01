#ifndef PYR_WOTS_H
#define PYR_WOTS_H

#include <stdint.h>

/**
 * WOTS key generation. Takes a 32 byte seed for the private key, expands it to
 * a full WOTS private key and computes the corresponding public key.
 * It requires the seed pub_seed (used to generate bitmasks and hash keys)
 * and the address of this WOTS key pair.
 *
 * Writes the computed public key to 'pk'.
 */
void wots_pkgen(unsigned char *pk, const unsigned char *seed,
                const unsigned char *pub_seed, uint32_t addr[8]);

/**
 * Takes a n-byte message and the 32-byte seed for the private key to compute a
 * signature that is placed at 'sig'.
 */
void wots_sign(unsigned char *sig, const unsigned char *msg,
               const unsigned char *seed, const unsigned char *pub_seed,
               uint32_t addr[8]);

/**
 * Takes a WOTS signature and an n-byte message, computes a WOTS public key.
 * Writes the computed public key to 'pk'.
 */
void wots_pk_from_sig(unsigned char *pk, const unsigned char *sig,
                      const unsigned char *msg, const unsigned char *pub_seed,
                      uint32_t addr[8]);

/**
 * Computes the leaf at a given address. First generates the WOTS key pair,
 * then computes leaf using l_tree. As this happens position independent, we
 * only require that addr encodes the right ltree-address.
 */
void gen_leaf_wots(unsigned char *leaf, const unsigned char *sk_seed,
                   const unsigned char *pub_seed, uint32_t wotspk_addr[8],
                   uint32_t ots_addr[8]);

/*
   The following functions have their static modifier removed for tests.
*/
void expand_seed(unsigned char *outseeds, const unsigned char *inseed,
                 const unsigned char *pub_seed, const uint32_t addr[8]);

void gen_chain(unsigned char *out, const unsigned char *in, unsigned int start,
               unsigned int steps, const unsigned char *pub_seed,
               uint32_t addr[8]);

void base_w(unsigned int *output, const int out_len,
            const unsigned char *input);

void wots_checksum(unsigned int *csum_base_w, const unsigned int *msg_base_w);

void chain_lengths(unsigned int *lengths, const unsigned char *msg);

/*
  Jasmin testing export function definitions.
*/
void wots_pkgen_jazz(unsigned char *pk, const unsigned char *seed,
                     const unsigned char *pub_seed, uint32_t addr[8]);

void wots_sign_jazz(unsigned char *sig, const unsigned char *msg,
                    const unsigned char *seed, const unsigned char *pub_seed,
                    uint32_t addr[8]);

void wots_pk_from_sig_jazz(unsigned char *pk, const unsigned char *sig,
                           const unsigned char *msg,
                           const unsigned char *pub_seed, uint32_t addr[8]);

void gen_leaf_wots_jazz(unsigned char *leaf, const unsigned char *sk_seed,
                        const unsigned char *pub_seed, uint32_t wotspk_addr[8],
                        uint32_t ots_addr[8]);

void expand_seed_jazz(unsigned char *outseeds, const unsigned char *inseed,
                      const unsigned char *pub_seed, const uint32_t addr[8]);

void gen_chain_jazz(unsigned char *out, unsigned int start, unsigned int steps,
                    const unsigned char *pub_seed, uint32_t addr[8]);

void base_w_m_jazz(unsigned int *output, const unsigned char *input);

void base_w_c_jazz(unsigned int *output, const unsigned char *input);

void wots_checksum_jazz(unsigned int *csum_base_w,
                        const unsigned int *msg_base_w);

void chain_lengths_jazz(unsigned int *lengths, const unsigned char *msg);

#endif

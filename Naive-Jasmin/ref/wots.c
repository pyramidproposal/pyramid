#include <stdint.h>
#include <string.h>

#include "address.h"
#include "hash.h"
#include "params.h"
#include "thash.h"
#include "convert.h"
#include "wots.h"

void expand_seed(unsigned char *outseeds,
		 const unsigned char *inseed,
		 const unsigned char *pub_seed, const uint32_t addr[8]){
  uint32_t i;
  uint32_t addr_[8] = {0};

  copy_keypair_addr(addr_, addr);
  set_type(addr_, PYR_ADDR_TYPE_PRFKG);
  
  for (i = 0; i < PYR_WOTS_LEN; i++) {
    set_chain_addr(addr_, i);
    prf(outseeds + i * PYR_N, inseed, pub_seed, addr_);
  }
}

/**
 * Computes the chaining function.
 * out and in have to be n-byte arrays.
 *
 * Interprets in as start-th value of the chain.
 * addr has to contain the address of the chain.
 */
void gen_chain(unsigned char *out, const unsigned char *in,
                      unsigned int start, unsigned int steps,
                      const unsigned char *pub_seed, uint32_t addr[8]) {
  uint32_t i;

  /* Initialize out with the value at position 'start'. */
  memcpy(out, in, PYR_N);

  /* Iterate 'steps' calls to the hash function. */
  for (i = start; i < (start + steps) && i < PYR_WOTS_W; i++) {
    set_hash_addr(addr, i);
    thash(out, out, 1, pub_seed, addr);
  }
}

/**
 * base_w algorithm as described in draft.
 * Interprets an array of bytes as integers in base w.
 * This only works when log_w is a divisor of 8.
 */
void base_w(unsigned int *output, const int out_len,
                   const unsigned char *input) {
  int in = 0;
  int out = 0;
  unsigned char total;
  int bits = 0;
  int consumed;

  for (consumed = 0; consumed < out_len; consumed++) {
    if (bits == 0) {
      total = input[in];
      in++;
      bits += 8;
    }
    bits -= PYR_WOTS_LOGW;
    output[out] = (total >> bits) & (PYR_WOTS_W - 1);
    out++;
  }
}

/* Computes the WOTS+ checksum over a message (in base_w). */
void wots_checksum(unsigned int *csum_base_w,
                          const unsigned int *msg_base_w) {
  unsigned int csum = 0;
  unsigned char csum_bytes[(PYR_WOTS_LEN2 * PYR_WOTS_LOGW + 7) / 8];
  unsigned int i;

  /* Compute checksum. */
  for (i = 0; i < PYR_WOTS_LEN1; i++) {
    csum += PYR_WOTS_W - 1 - msg_base_w[i];
  }

  /* Convert checksum to base_w. */
  /* Make sure expected empty zero bits are the least significant bits. */
  csum = csum << ((8 - ((PYR_WOTS_LEN2 * PYR_WOTS_LOGW) % 8)) % 8);
  ull_to_bytes(csum_bytes, sizeof(csum_bytes), csum);
  base_w(csum_base_w, PYR_WOTS_LEN2, csum_bytes);
}

/* Takes a message and derives the matching chain lengths. */
void chain_lengths(unsigned int *lengths, const unsigned char *msg) {
  base_w(lengths, PYR_WOTS_LEN1, msg);
  wots_checksum(lengths + PYR_WOTS_LEN1, lengths);
}

/**
 * WOTS key generation. Takes a 32 byte seed for the private key, expands it to
 * a full WOTS private key and computes the corresponding public key.
 * It requires the seed pub_seed (used to generate bitmasks and hash keys)
 * and the address of this WOTS key pair.
 *
 * Writes the computed public key to 'pk'.
 */
void wots_pkgen(unsigned char *pk, const unsigned char *seed,
                const unsigned char *pub_seed, uint32_t addr[8]) {
  uint32_t i;

  /* The WOTS+ private key is derived from the seed. */
  expand_seed(pk, seed, pub_seed, addr);

  for (i = 0; i < PYR_WOTS_LEN; i++) {
    set_chain_addr(addr, i);
    gen_chain(pk + i * PYR_N, pk + i * PYR_N, 0, PYR_WOTS_W - 1, pub_seed,
              addr);
  }
}

/**
 * Takes a n-byte message and the 32-byte seed for the private key to compute a
 * signature that is placed at 'sig'.
 */
void wots_sign(unsigned char *sig, const unsigned char *msg,
               const unsigned char *seed, const unsigned char *pub_seed,
               uint32_t addr[8]) {
  unsigned int lengths[PYR_WOTS_LEN];
  uint32_t i;

  chain_lengths(lengths, msg);

  /* The WOTS+ private key is derived from the seed. */
  expand_seed(sig, seed, pub_seed, addr);

  for (i = 0; i < PYR_WOTS_LEN; i++) {
    set_chain_addr(addr, i);
    gen_chain(sig + i * PYR_N, sig + i * PYR_N, 0, lengths[i], pub_seed, addr);
  }
}

/**
 * Takes a WOTS signature and an n-byte message, computes a WOTS public key.
 *
 * Writes the computed public key to 'pk'.
 */
void wots_pk_from_sig(unsigned char *pk, const unsigned char *sig,
                      const unsigned char *msg, const unsigned char *pub_seed,
                      uint32_t addr[8]) {
  unsigned int lengths[PYR_WOTS_LEN];
  uint32_t i;

  chain_lengths(lengths, msg);

  for (i = 0; i < PYR_WOTS_LEN; i++) {
    set_chain_addr(addr, i);
    gen_chain(pk + i * PYR_N, sig + i * PYR_N, lengths[i],
              PYR_WOTS_W - 1 - lengths[i], pub_seed, addr);
  }
}

/**
 * Computes the leaf at a given address. First generates the WOTS key pair,
 * then computes leaf using l_tree. As this happens position independent, we
 * only require that addr encodes the right ltree-address.
 */
void gen_leaf_wots(unsigned char *leaf, const unsigned char *sk_seed,
                   const unsigned char *pub_seed, uint32_t wotspk_addr[8],
                   uint32_t ots_addr[8]) {
  unsigned char pk[PYR_WOTS_PK_BYTES];

  wots_pkgen(pk, sk_seed, pub_seed, ots_addr);

  thash(leaf, pk, PYR_WOTS_LEN, pub_seed, wotspk_addr);
}

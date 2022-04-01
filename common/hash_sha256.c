#include "hash.h"

#include <stdint.h>
#include <string.h>

#include "address.h"
#include "params.h"
#include "sha256.h"

/* sha512 is currently not included in the proposal,
   but we include it in h_msg in line with the SPHINCS+ code. */
#if PYR_N == 32
#define PYR_SHAX_OUTPUT_BYTES PYR_SHA512_OUTPUT_BYTES
#define PYR_SHAX_BLOCK_BYTES PYR_SHA512_BLOCK_BYTES
#define shaX_inc_init sha512_inc_init
#define shaX_inc_blocks sha512_inc_blocks
#define shaX_inc_finalize sha512_inc_finalize
#define shaX sha512
#define mgf1_X mgf1_512
#else
#define PYR_SHAX_OUTPUT_BYTES PYR_SHA256_OUTPUT_BYTES
#define PYR_SHAX_BLOCK_BYTES PYR_SHA256_BLOCK_BYTES
#define shaX_inc_init sha256_inc_init
#define shaX_inc_blocks sha256_inc_blocks
#define shaX_inc_finalize sha256_inc_finalize
#define shaX sha256
#define mgf1_X mgf1_256
#endif

/* Code that implements ONE hash instantiation, such as FSG,
   can create an address copy within the buffer using this.
   When using a precomputed state, we subtract PYR_SHAX_BLOCK_BYTES. */
#define ADDR_POS_256 PYR_SHA256_BLOCK_BYTES
#define ADDR_POS_512 PYR_SHA512_BLOCK_BYTES
#define ADDR_POS_256_PRE (ADDR_POS_256 - PYR_SHA256_BLOCK_BYTES)

#if ADDR_POS_256_PRE != 0
#error \
    "From here on out, we assume ADDR_POS_256_PRE = 0, to preserve readability."
#endif

/**
 * Embeds the public seed into a static state.
 */
void initialize_hash_function(const unsigned char *pk_seed,
                              const unsigned char *sk_seed) {
  seed_state(pk_seed);
  (void)sk_seed;
}

/**
 * PRF: SHA256(S_PK.Seed || ADRS^C  || SK.PRF)
 * ADRS expectation: completed.
 * We reuse the intermediate SHA-256 state.
 */
void prf(unsigned char *out, const unsigned char *key,
         const unsigned char *pub_seed, const uint32_t addr[8]) {
  unsigned char buf[PYR_SHA256_ADDR_BYTES + PYR_N];
  unsigned char outbuf[PYR_SHA256_OUTPUT_BYTES];
  uint8_t sha2_state[40];

  /* Retrieve precomputed state containing pub_seed */
  memcpy(sha2_state, state_seeded, 40 * sizeof(uint8_t));
  (void)pub_seed;

  memcpy(buf, addr, PYR_SHA256_ADDR_BYTES);
  memcpy(buf + PYR_SHA256_ADDR_BYTES, key, PYR_N);

  sha256_inc_finalize(outbuf, sha2_state, buf, PYR_SHA256_ADDR_BYTES + PYR_N);

  memcpy(out, outbuf, PYR_N);
}

/**
 * h_msg: SHA256(R || PK.Root || toByte(0, 64 − 2n) || ADRS^C || M )
 * ADRS expectation: completed.
 */
void h_msg(unsigned char *out, const unsigned char *R,
           const unsigned char *root, const uint32_t addr[8],
           const unsigned char *m, size_t mlen) {
  /* The combined bytelength of constant-length fields. */
#define H_MSG_CONST_INLEN (PYR_SHAX_BLOCK_BYTES + PYR_SHA256_ADDR_BYTES)
  /* The nearest multiple of PYR_SHAX_BLOCK_BYTES required for H_MSG_CONST_INLEN
   * bytes. */
#define PYR_INBLOCKS                                                          \
  (((H_MSG_CONST_INLEN + PYR_SHAX_BLOCK_BYTES - 1) & -PYR_SHAX_BLOCK_BYTES) / \
   PYR_SHAX_BLOCK_BYTES)

  unsigned char inbuf[PYR_INBLOCKS * PYR_SHAX_BLOCK_BYTES] = {0}; /* pad */
  unsigned char outbuf[PYR_SHAX_OUTPUT_BYTES];
  uint8_t state[8 + PYR_SHAX_OUTPUT_BYTES];

  shaX_inc_init(state);

  memcpy(inbuf, R, PYR_N);
  memcpy(inbuf + PYR_N, root, PYR_N);
  memcpy(inbuf + PYR_SHAX_BLOCK_BYTES, addr, PYR_SHA256_ADDR_BYTES);

  /* If the entire input fits within PYR_INBLOCKS blocks. */
  if (H_MSG_CONST_INLEN + mlen < PYR_INBLOCKS * PYR_SHAX_BLOCK_BYTES) {
    memcpy(inbuf + H_MSG_CONST_INLEN, m, mlen);
    shaX_inc_finalize(outbuf, state, inbuf, H_MSG_CONST_INLEN + mlen);
  }
  /* Otherwise first top off block PYR_INBLOCKS, so that finalize only uses the
     message */
  else {
    memcpy(inbuf + H_MSG_CONST_INLEN, m,
           PYR_INBLOCKS * PYR_SHAX_BLOCK_BYTES - H_MSG_CONST_INLEN);
    shaX_inc_blocks(state, inbuf, PYR_INBLOCKS);

    m += PYR_INBLOCKS * PYR_SHAX_BLOCK_BYTES - H_MSG_CONST_INLEN;
    mlen -= PYR_INBLOCKS * PYR_SHAX_BLOCK_BYTES - H_MSG_CONST_INLEN;
    shaX_inc_finalize(outbuf, state, m, mlen);
  }
  memcpy(out, outbuf, PYR_N);
}

/**
 * FSG: SHA256(S_PK.Seed || ADRS^C || SK.Seed_i )
 * ADRS expectation: layer address, tree address, key pair address.
 * We reuse the intermediate SHA-256 state.
 */
void fsg(unsigned char *out, unsigned char *next_seed, const unsigned char *in,
         const unsigned char *pub_seed, const uint32_t addr[8]) {
  unsigned char buf[PYR_SHA256_ADDR_BYTES + PYR_N] = {0}; /* pad */
  unsigned char outbuf[PYR_SHA256_OUTPUT_BYTES];
  uint8_t sha2_state[40];

  /* Retrieve precomputed state containing pub_seed */
  memcpy(sha2_state, state_seeded, 40 * sizeof(uint8_t));
  (void)pub_seed;

  copy_keypair_addr((uint32_t *)buf, addr);
  set_type((uint32_t *)buf, PYR_ADDR_TYPE_FSG);

  memcpy(buf + PYR_SHA256_ADDR_BYTES, in, PYR_N);

  if (out != NULL) {
    set_direction((uint32_t *)buf, 0);
    sha256_inc_finalize(outbuf, sha2_state, buf, PYR_SHA256_ADDR_BYTES + PYR_N);
    memcpy(out, outbuf, PYR_N);
  }

  if (next_seed != NULL) {
    if (out != NULL) { /* then sha2_state has changed */
      memcpy(sha2_state, state_seeded, 40 * sizeof(uint8_t));
    }
    set_direction((uint32_t *)buf, 1);
    sha256_inc_finalize(outbuf, sha2_state, buf, PYR_SHA256_ADDR_BYTES + PYR_N);
    memcpy(next_seed, outbuf, PYR_N);
  }
}

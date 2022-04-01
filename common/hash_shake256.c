#include "hash.h"

#include <stdint.h>
#include <string.h>

#include "address.h"
#include "fips202.h"
#include "params.h"

/* Code that implements ONE hash instantiation, such as FSG,
   can create an address copy within the buffer using this. */
#define ADDR_POS PYR_N

/**
 * For SHAKE256, there is no immediate reason to initialize at the start,
 * so this function is an empty operation.
 */
void initialize_hash_function(const unsigned char *pk_seed,
                              const unsigned char *sk_seed) {
  (void)pk_seed;
  (void)sk_seed;
}

/**
 * PRF: SHAKE256(SK.PRF || ADRS || PK.Seed, 8n)
 * ADRS expectation: completed.
 *
 * We use a single PRF that includes pub_seed, to keep the code simple.
 */
void prf(unsigned char *out, const unsigned char *key,
         const unsigned char *pub_seed, const uint32_t addr[8]) {
  unsigned char buf[2 * PYR_N + 32];

  memcpy(buf, key, PYR_N);
  memcpy(buf + PYR_N, addr, 32);
  memcpy(buf + PYR_N + 32, pub_seed, PYR_N);

  shake256(out, PYR_N, buf, 2 * PYR_N + 32);
}

/**
 * h_msg: SHAKE256(R || ADRS || PK.Root || M, 8m)
 * ADRS expectation: completed.
 */
void h_msg(unsigned char *out, const unsigned char *R,
           const unsigned char *root, const uint32_t addr[8],
           const unsigned char *m, size_t mlen) {
  uint64_t s_inc[26];

  shake256_inc_init(s_inc);
  shake256_inc_absorb(s_inc, R, PYR_N);
  shake256_inc_absorb(s_inc, (unsigned char *)addr, 32);
  shake256_inc_absorb(s_inc, root, PYR_N);
  shake256_inc_absorb(s_inc, m, mlen);
  shake256_inc_finalize(s_inc);
  shake256_inc_squeeze(out, PYR_N, s_inc);
}

/**
 * FSG: SHAKE256(SK.Seed_i || ADRS || PK.Seed, 8n)
 * ADRS expectation: layer address, tree address, key pair address.
 */
void fsg(unsigned char *out, unsigned char *next_seed, const unsigned char *in,
         const unsigned char *pub_seed, const uint32_t addr[8]) {
  unsigned char buf[2 * PYR_N + 32] = {0}; /* pad */

  copy_keypair_addr((uint32_t *)(buf + ADDR_POS), addr);
  set_type((uint32_t *)(buf + ADDR_POS), PYR_ADDR_TYPE_FSG);

  memcpy(buf, in, PYR_N);
  /* buf + PYR_N: address */
  memcpy(buf + PYR_N + 32, pub_seed, PYR_N);

  if (out != NULL) {
    set_direction((uint32_t *)(buf + ADDR_POS), 0);
    shake256(out, PYR_N, buf, 2 * PYR_N + 32);
  }

  if (next_seed != NULL) {
    set_direction((uint32_t *)(buf + ADDR_POS), 1);
    shake256(next_seed, PYR_N, buf, 2 * PYR_N + 32);
  }
}

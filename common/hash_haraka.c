#include "hash.h"

#include <stdint.h>
#include <string.h>

#include "address.h"
#include "haraka.h"
#include "params.h"

/* Code that implements ONE hash instantiation, such as FSG,
   can create an address copy within the buffer using this. */
#define ADDR_POS 0

#if ADDR_POS != 0
#error "From here on out, we assume ADDR_POS = 0, to preserve readability."
#endif

/**
 * If PRF_keygen(Seed, ADRS) = Haraka256_Seed(ADRS), like in the proposal,
 * then one may want to re-key sk_seed for every WOTS-TW sk generation.
 * In "tweak_constants", this requires an additional guard
 * "if (pk_seed != NULL)", because pk_seed is constant.
 */
void initialize_hash_function(const unsigned char *pk_seed,
                              const unsigned char *sk_seed) {
  (void)sk_seed;
  tweak_constants(pk_seed, NULL, PYR_N);
}

/**
 * PRF: Haraka512_PK.Seed(ADRS || SK.Seed, 8n)
 */
void prf(unsigned char *out, const unsigned char *key,
         const unsigned char *pub_seed, const uint32_t addr[8]) {
  unsigned char buf[64] = {0}; /* pad */
  unsigned char outbuf[32];

  (void)pub_seed;
  memcpy(buf, addr, 32);
  memcpy(buf + 32, key, PYR_N);

  haraka512(outbuf, buf);
  memcpy(out, outbuf, PYR_N);
}

/**
 * H_msg: HarakaS_PK.Seed (ADRS || R || PK.Root || M, 8m)
 */
void h_msg(unsigned char *out, const unsigned char *R,
           const unsigned char *root, const uint32_t addr[8],
           const unsigned char *m, size_t mlen) {
  uint8_t s_inc[65];

  haraka_S_inc_init(s_inc);
  haraka_S_inc_absorb(s_inc, (const unsigned char *)addr, 32);
  haraka_S_inc_absorb(s_inc, R, PYR_N);
  haraka_S_inc_absorb(s_inc, root, PYR_N);
  haraka_S_inc_absorb(s_inc, m, mlen);
  haraka_S_inc_finalize(s_inc);
  haraka_S_inc_squeeze(out, PYR_N, s_inc);
}

/**
 * HPRG: Haraka512_PK.Seed(ADRS || SK.Seed_i, 8n)
 */
void fsg(unsigned char *out, unsigned char *next_seed, const unsigned char *in,
         const unsigned char *pub_seed, const uint32_t addr[8]) {
  unsigned char buf[64] = {0}; /* pad */
  unsigned char outbuf[32];

  copy_keypair_addr((uint32_t *)buf, addr);
  set_type((uint32_t *)buf, PYR_ADDR_TYPE_FSG);

  (void)pub_seed;
  /* buf + 0: address */
  memcpy(buf + 32, in, PYR_N);

  if (out != NULL) {
    set_direction((uint32_t *)buf, 0);
    haraka512(outbuf, buf);
    memcpy(out, outbuf, PYR_N);
  }

  if (next_seed != NULL) {
    set_direction((uint32_t *)buf, 1);
    haraka512(outbuf, buf);
    memcpy(next_seed, outbuf, PYR_N);
  }
}

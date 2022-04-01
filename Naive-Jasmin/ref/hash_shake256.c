#include <stdint.h>
#include <string.h>

#include "hash.h"
#include "params.h"
#include "fips202.h"

/* Code that implements ONE hash instantiation, such as FSG,
   can create an address copy within the buffer using this. */
#define ADDR_POS PYR_N

/**
 * PRF: SHAKE256(SK.PRF || ADRS || PK.Seed, 8n)
 * ADRS expectation: completed.
 *
 * We use a single PRF that includes pub_seed, to keep the code simple.
 */
void prf(unsigned char *out, const unsigned char *key,
	 const unsigned char *pub_seed, const uint32_t addr[8]) {
  unsigned char buf[2*PYR_N + 32];

  memcpy(buf, key, PYR_N);
  memcpy(buf + PYR_N, addr, 32);
  memcpy(buf + PYR_N + 32, pub_seed, PYR_N);

  shake256(out, PYR_N, buf, 2*PYR_N + 32);
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

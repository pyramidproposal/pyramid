#include "thash.h"

#include <stdint.h>
#include <string.h>

#include "address.h"
#include "params.h"
#include "sha256.h"

/**
 * Takes an array of inblocks concatenated arrays of PYR_N bytes.
 */
void thash(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, const uint32_t addr[8]) {
  unsigned char buf[PYR_SHA256_ADDR_BYTES + inblocks * PYR_N];
  unsigned char outbuf[PYR_SHA256_OUTPUT_BYTES];
  uint8_t sha2_state[40];

  (void)pub_seed; /* Suppress an 'unused parameter' warning. */

  /* Retrieve precomputed state containing pub_seed */
  memcpy(sha2_state, state_seeded, 40 * sizeof(uint8_t));

  memcpy(buf, addr, PYR_SHA256_ADDR_BYTES);
  memcpy(buf + PYR_SHA256_ADDR_BYTES, in, inblocks * PYR_N);

  sha256_inc_finalize(outbuf, sha2_state, buf,
                      PYR_SHA256_ADDR_BYTES + inblocks * PYR_N);
  memcpy(out, outbuf, PYR_N);
}

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
  unsigned char buf[PYR_N + PYR_SHA256_ADDR_BYTES + inblocks * PYR_N];
  unsigned char outbuf[PYR_SHA256_OUTPUT_BYTES];
  unsigned char bitmask[inblocks * PYR_N];
  uint8_t sha2_state[40];
  unsigned int i;

  memcpy(buf, pub_seed, PYR_N);
  memcpy(buf + PYR_N, addr, PYR_SHA256_ADDR_BYTES);
  mgf1_256(bitmask, inblocks * PYR_N, buf, PYR_N + PYR_SHA256_ADDR_BYTES);

  /* Retrieve precomputed state containing pub_seed */
  memcpy(sha2_state, state_seeded, 40 * sizeof(uint8_t));

  for (i = 0; i < inblocks * PYR_N; i++) {
    buf[PYR_N + PYR_SHA256_ADDR_BYTES + i] = in[i] ^ bitmask[i];
  }

  sha256_inc_finalize(outbuf, sha2_state, buf + PYR_N,
                      PYR_SHA256_ADDR_BYTES + inblocks * PYR_N);
  memcpy(out, outbuf, PYR_N);
}

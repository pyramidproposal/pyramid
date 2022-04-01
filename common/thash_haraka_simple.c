#include "thash.h"

#include <stdint.h>
#include <string.h>

#include "address.h"
#include "haraka.h"
#include "params.h"

/**
 * Takes an array of inblocks concatenated arrays of PYR_N bytes.
 */
void thash(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, const uint32_t addr[8]) {
  unsigned char buf[PYR_ADDR_BYTES + inblocks * PYR_N];
  unsigned char outbuf[32];
  unsigned char buf_tmp[64];

  (void)pub_seed; /* Suppress an 'unused parameter' warning. */

  if (inblocks == 1) {
    /* F function */
    /* Since PYR_N may be smaller than 32, we need a temporary buffer. */
    memset(buf_tmp, 0, 64);
    memcpy(buf_tmp, addr, 32);
    memcpy(buf_tmp + PYR_ADDR_BYTES, in, PYR_N);

    haraka512(outbuf, buf_tmp);
    memcpy(out, outbuf, PYR_N);
  } else {
    /* All other tweakable hashes*/
    memcpy(buf, addr, 32);
    memcpy(buf + PYR_ADDR_BYTES, in, inblocks * PYR_N);

    haraka_S(out, PYR_N, buf, PYR_ADDR_BYTES + inblocks * PYR_N);
  }
}

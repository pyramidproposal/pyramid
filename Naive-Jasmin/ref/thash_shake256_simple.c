#include <stdint.h>
#include <string.h>

#include "thash.h"
#include "params.h"

#include "fips202.h"

/**
 * Takes an array of inblocks concatenated arrays of PYR_N bytes.
 */
void thash(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, const uint32_t addr[8])
{
    unsigned char buf[PYR_N + PYR_ADDR_BYTES + inblocks*PYR_N];

    memcpy(buf, pub_seed, PYR_N);
    memcpy(buf + PYR_N, addr, PYR_ADDR_BYTES);
    memcpy(buf + PYR_N + PYR_ADDR_BYTES, in, inblocks * PYR_N);

    shake256(out, PYR_N, buf, PYR_N + PYR_ADDR_BYTES + inblocks*PYR_N);
}

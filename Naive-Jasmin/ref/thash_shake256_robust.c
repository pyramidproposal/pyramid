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
    unsigned char bitmask[inblocks * PYR_N];
    unsigned int i;

    memcpy(buf, pub_seed, PYR_N);
    memcpy(buf + PYR_N, addr, PYR_ADDR_BYTES);

    shake256(bitmask, inblocks * PYR_N, buf, PYR_N + PYR_ADDR_BYTES);

    for (i = 0; i < inblocks * PYR_N; i++) {
        buf[PYR_N + PYR_ADDR_BYTES + i] = in[i] ^ bitmask[i];
    }

    shake256(out, PYR_N, buf, PYR_N + PYR_ADDR_BYTES + inblocks*PYR_N);
}

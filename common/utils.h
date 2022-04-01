#ifndef PYR_UTILS_H
#define PYR_UTILS_H

#include <stdint.h>

void fwd_ref(unsigned char *next_seed, const unsigned char *sk_seed,
             const unsigned char *pub_seed, uint32_t nr_apply,
             const uint32_t start_addr[8]);

#endif

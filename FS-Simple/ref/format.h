#ifndef PYR_FORMAT_H
#define PYR_FORMAT_H

#include <stddef.h>
#include <stdint.h>

#include "params.h"

/* Ensure that ext_algo.h is aware of the FORWARD_SECURE toggle. */
#include "ext_algo.h"

#if PYR_FULL_HEIGHT > 64 /* 1. Last index in tree with indices below 2^64. */
#define LAST_VALID_IDX \
  ((1ULL << ((64 / PYR_TREE_HEIGHT) * PYR_TREE_HEIGHT)) - 2)
#elif PYR_FULL_HEIGHT == \
    64 /* 2. Extreme case PYR_TREE_HEIGHT = 1, PYR_FULL_HEIGHT = 64. */
#define LAST_VALID_IDX (UINT64_MAX - 1)
#else /* 3. The index uses < 64 bits, then one of two: */
#if (PYR_FULL_HEIGHT % 8) == 0 /* 3.1. One invalid index. */
#define LAST_VALID_IDX ((1ULL << PYR_FULL_HEIGHT) - 2)
#else /* 3.2. (PYR_FULL_HEIGHT % 8) invalid indices. */
#define LAST_VALID_IDX ((1ULL << PYR_FULL_HEIGHT) - 1)
#endif
#endif

#ifdef FORWARD_SECURE
#define NR_SSD PYR_D
#else
#define NR_SSD 1
#endif

#define PK_RT 0
#define PK_SD (PK_RT + PYR_N)

#define SK_IDX 0
#define SK_SSD (SK_IDX + PYR_IDX_BYTES)
#define SK_PRF (SK_SSD + PYR_N * NR_SSD)
#define SK_PRT (SK_PRF + PYR_N + PK_RT)
#define SK_PSD (SK_PRT + PK_SD)

#define SIG_IDX 0
#define SIG_RND (SIG_IDX + PYR_IDX_BYTES)
#define SIG_HTS (SIG_RND + PYR_N)

/* Resulting PYR sizes. */
#define PYR_SK_CORE_BYTES (SK_PSD + PYR_N)
#define PYR_BYTES (SIG_HTS + PYR_FULL_HEIGHT * PYR_N + PYR_D * PYR_WOTS_BYTES)
#define PYR_PK_BYTES (2 * PYR_N)
#define PYR_SK_BYTES (PYR_SK_CORE_BYTES + SK_EXT_ALGO_BYTES)

int last_idxp(uint64_t idx);

int invalid_idxp(uint64_t idx);

void delete_sk(unsigned char *sk);

void forget_refs(unsigned char *sk);

void sig_mhash(unsigned char *mhash, unsigned char *r, const unsigned char *sk,
               const unsigned char *m, size_t mlen);

void sig_base(unsigned char *sig, size_t *sig_len, const unsigned char *r,
              uint64_t idx_sig);

#endif

#ifndef PYR_FORMAT_H
#define PYR_FORMAT_H

#include <stddef.h>
#include <stdint.h>

#include "params.h"

#if PYR_FULL_HEIGHT > 64
#define LAST_VALID_IDX \
  ((1ULL << ((64 / PYR_TREE_HEIGHT) * PYR_TREE_HEIGHT)) - 2)
#elif PYR_FULL_HEIGHT == 64
#define LAST_VALID_IDX (UINT64_MAX - 1)
#else
#if (PYR_FULL_HEIGHT % 8) == 0
#define LAST_VALID_IDX ((1ULL << PYR_FULL_HEIGHT) - 2)
#else
#define LAST_VALID_IDX ((1ULL << PYR_FULL_HEIGHT) - 1)
#endif
#endif

#define PYR_SK_CORE_BYTES (PYR_IDX_BYTES + (PYR_D + 3) * PYR_N)

#define BDS_BYTES                                                          \
  ((PYR_TREE_HEIGHT * PYR_N) + 1 + ((PYR_TREE_HEIGHT - 1) * (PYR_N + 1)) + \
   (PYR_TREE_HEIGHT * (PYR_N + 1 + PYR_IDX_BYTES + 2 * PYR_N)) +           \
   ((PYR_TREE_HEIGHT / 2) * PYR_N))

#define MMT_BYTES                                                    \
  ((PYR_TREE_HEIGHT * PYR_N) + 1 + (PYR_TREE_HEIGHT * (PYR_N + 1)) + \
   (PYR_TREE_HEIGHT * (1 + PYR_IDX_BYTES + PYR_N)))

#define XMSSMT_STATE_BYTES (BDS_BYTES + (PYR_D - 1) * MMT_BYTES)

#define PYR_SK_BYTES (PYR_SK_CORE_BYTES + XMSSMT_STATE_BYTES)
#define PYR_BYTES \
  (PYR_IDX_BYTES + PYR_D * PYR_WOTS_BYTES + (PYR_FULL_HEIGHT + 1) * PYR_N)
#define PYR_PK_BYTES (2 * PYR_N)

#endif

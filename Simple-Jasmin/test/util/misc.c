#include <stdint.h>
#include <string.h>

#include "../../ref/convert.h"
#include "../../ref/format.h"
#include "../../ref/params.h"
#include "randomtestdata.h"

uint32_t random_test_s(uint32_t t) {
  uint32_t s;
  random_test_32bit_blocks(&s, 1);
  s &= (1 << PYR_TREE_HEIGHT) - 1;
  s &= ~((1 << t) - 1);
  return s;
}

void increment_sk_idx(unsigned char *sk) {
  uint64_t idx;
  idx = bytes_to_ull(sk + SK_IDX, PYR_IDX_BYTES);
  ull_to_bytes(sk + SK_IDX, PYR_IDX_BYTES, idx + 1);
}

void deleted_sk(unsigned char *sk) {
#if (PYR_TREE_HEIGHT % 8) != 0
  unsigned char pos;
  random_test_bytes(&pos, 1);
  random_test_bytes(sk + SK_IDX, PYR_IDX_BYTES);
  /* Now ensure that at least one of the upper 8 - (PYR_FULL_HEIGHT % 8) bits is
   * set. */
  pos %=
      8 - (PYR_FULL_HEIGHT % 8) - 1; /* 0 <= pos < 8 - (PYR_FULL_HEIGHT % 8) */
  sk[SK_IDX] |= (1 << (pos + (PYR_FULL_HEIGHT % 8)));
#else
  memset(sk + SK_IDX, 0xff, PYR_IDX_BYTES);
#endif

  memset(sk + PYR_IDX_BYTES, 0x00,
         PYR_SK_CORE_BYTES + SK_EXT_ALGO_BYTES - PYR_IDX_BYTES);
}

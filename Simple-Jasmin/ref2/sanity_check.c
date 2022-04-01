#include <assert.h>
#include <stdint.h>

#include "format.h"
#include "params.h"

void sanity_check_jazz(uint64_t paramp[10]);

int main() {
  uint64_t paramp[17];

  sanity_check_jazz(paramp);
  assert(paramp[0] == PYR_N);
  assert(paramp[1] == PYR_FULL_HEIGHT);
  assert(paramp[2] == PYR_D);
  assert(paramp[3] == PYR_WOTS_W);
  /* assert(paramp[4] == PYR_ADDR_BYTES); */
  assert(paramp[5] == PYR_IDX_BYTES);
  assert(paramp[6] == PYR_WOTS_LOGW);
  assert(paramp[7] == PYR_WOTS_LEN1);
  assert(paramp[8] == PYR_WOTS_LEN2);
  assert(paramp[9] == PYR_TREE_HEIGHT);

  assert(paramp[10] == PYR_SK_CORE_BYTES);
  assert(paramp[11] == BDS_BYTES);
  assert(paramp[12] == MMT_BYTES);
  assert(paramp[13] == XMSSMT_STATE_BYTES);
  assert(paramp[14] == PYR_SK_BYTES);
  assert(paramp[15] == PYR_BYTES);
  assert(paramp[16] == PYR_PK_BYTES);

  return 0;
}

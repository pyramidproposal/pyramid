#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/format.h"
#include "../ref/params.h"
#include "../ref/simple_MMT_state.h"
#include "../ref/pyr_simple.h"
#include "util/compare.h"
#include "util/randomtestdata.h"

/* MMT state */
#define STATE_TYPE 1

int test_MMT_stategen() {
  int res = 0;
  unsigned char sk[PYR_SK_CORE_BYTES];
  unsigned char mmt_state_c[MMT_BYTES];
  unsigned char mmt_state_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  random_test_bytes(sk, PYR_SK_CORE_BYTES);
  state_deserialize(mmt_state_c, &state_c, STATE_TYPE);

  mmt_stateGen(sk, &state_c, STATE_TYPE);
  MMT_stategen_jazz(mmt_state_jazz, sk, STATE_TYPE);

  state_serialize(mmt_state_c, &state_c, STATE_TYPE);
  res = cmp_MMT_state(mmt_state_c, mmt_state_jazz);

  return res;
}

int main() {
  int res = 0;

  res |= test_MMT_stategen();

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

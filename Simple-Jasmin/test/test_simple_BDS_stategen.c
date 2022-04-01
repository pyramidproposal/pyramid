#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/format.h"
#include "../ref/params.h"
#include "../ref/simple_BDS_state.h"
#include "../ref/pyr_simple.h"
#include "util/compare.h"
#include "util/randomtestdata.h"

/* BDS state */
#define STATE_TYPE 0

int test_bds_stategen() {
  int res = 0;
  unsigned char sk[PYR_SK_CORE_BYTES];
  unsigned char bds_state_c[BDS_BYTES];
  unsigned char bds_state_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  random_test_bytes(sk, PYR_SK_CORE_BYTES);
  state_deserialize(bds_state_c, &state_c, STATE_TYPE);

  bds_stateGen(sk, &state_c);
  BDS_stategen_jazz(bds_state_jazz, sk);

  state_serialize(bds_state_c, &state_c, STATE_TYPE);

  res = cmp_BDS_state(bds_state_c, bds_state_jazz);

  return res;
}

int main() {
  int res = 0;

  res |= test_bds_stategen();

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

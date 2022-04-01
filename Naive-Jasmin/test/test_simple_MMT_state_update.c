#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/api.h"
#include "../ref/params.h"
#include "../ref/xmss_simple.h"
#include "util/compare.h"
#include "util/misc.h"
#include "util/randomtestdata.h"

/* MMT state */
#define STATE_TYPE 1
#define NR_TEST (3 * (1ULL << PYR_TREE_HEIGHT))

int test_MMT_state_update() {
  int res = 0;
  size_t j;
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char *sk_mmt = sk + PYR_SK_CORE_BYTES + BDS_BYTES;
  unsigned char _[PYR_PK_BYTES];
  unsigned char mmt_state_c[MMT_BYTES];
  unsigned char mmt_state_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;

  /* Obtain a valid keypair. */
  crypto_sign_keypair(_, sk);

  for (j = 0; j < NR_TEST; ++j) {
    /* Copy MMT state into standalone MMT states. */
    memcpy(mmt_state_c, sk_mmt, MMT_BYTES);
    memcpy(mmt_state_jazz, sk_mmt, MMT_BYTES);

    state_deserialize(mmt_state_c, &state_c, STATE_TYPE);

    mmt_state_update(sk, &state_c, STATE_TYPE);
    MMT_state_update_jazz(mmt_state_jazz, sk, STATE_TYPE);

    state_serialize(mmt_state_c, &state_c, STATE_TYPE);

    res = cmp_MMT_state(mmt_state_c, mmt_state_jazz);
    if (res) {
      printf("[!] Unit test failed.\nFunction:\tBDS_state_update (%zu).\n", j);
    }

    /* Copy updated standalone back into MMT state. */
    memcpy(sk_mmt, mmt_state_c, MMT_BYTES);

    /* Update sk for the next round. */
    forget_seeds(sk);
    increment_sk_idx(sk);
  }

  return res;
}

int main() {
  int res = 0;

  res |= test_MMT_state_update();

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

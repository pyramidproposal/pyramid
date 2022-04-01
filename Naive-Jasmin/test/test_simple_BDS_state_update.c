#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/api.h"
#include "../ref/params.h"
#include "../ref/xmss_simple.h"
#include "util/compare.h"
#include "util/misc.h"
#include "util/randomtestdata.h"

/* BDS state */
#define STATE_TYPE 0
#define NR_TEST (3 * (1ULL << PYR_TREE_HEIGHT))

int test_BDS_state_update() {
  int res = 0;
  size_t j;
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char *sk_bds = sk + PYR_SK_CORE_BYTES;
  unsigned char _[PYR_PK_BYTES];
  unsigned char bds_state_c[BDS_BYTES];
  unsigned char bds_state_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;

  /* Obtain a valid keypair. */
  crypto_sign_keypair(_, sk);

  for (j = 0; j < NR_TEST && res; ++j) {
    /* Copy BDS state into standalone BDS states. */
    memcpy(bds_state_c, sk_bds, BDS_BYTES);
    memcpy(bds_state_jazz, sk_bds, BDS_BYTES);

    state_deserialize(bds_state_c, &state_c, STATE_TYPE);

    bds_state_update(sk, &state_c);
    BDS_state_update_jazz(bds_state_jazz, sk);

    state_serialize(bds_state_c, &state_c, STATE_TYPE);

    res = cmp_BDS_state(bds_state_c, bds_state_jazz);

    /* Copy updated standalone back into BDS state. */
    memcpy(sk_bds, bds_state_c, BDS_BYTES);

    /* Update sk for the next round. */
    forget_seeds(sk);
    increment_sk_idx(sk);
  }

  if (res) {
    printf("[!] Unit test failed.\nFunction:\tBDS_state_update (%zu).\n", j);
  }

  return res;
}

int main() {
  int res = 0;

  res |= test_BDS_state_update();

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

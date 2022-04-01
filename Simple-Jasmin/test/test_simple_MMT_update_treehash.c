#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/params.h"
#include "../ref/pyr_simple.h"
#include "util/compare.h"
#include "util/misc.h"
#include "util/randomtestdata.h"

/* MMT state */
#define STATE_TYPE 1
#define NR_TEST 5000
#define MAX_HEIGHT (PYR_TREE_HEIGHT - 1)

int test_MMT_update_treehash_tree() {
  int res = 0;
  uint32_t s, t;
  unsigned char sk_psd[PYR_N];
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;

  /* Set up randomized state in sk_c. */
  random_test_bytes(sk_c, MMT_BYTES);
  memcpy(sk_jazz, sk_c, MMT_BYTES);
  random_test_bytes(sk_psd, PYR_N);
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  s = 0;
  t = PYR_TREE_HEIGHT - 1;
  state_c.stack->size = 0;
  (state_c.treehash + t)->fin = 0;
  (state_c.treehash + t)->idx = s;

  /* Sync struct changes to sk_jazz. */
  state_serialize(sk_jazz, &state_c, STATE_TYPE);

  update_treehash(sk_psd, t, &state_c, STATE_TYPE);
  MMT_update_treehash_jazz(sk_psd, t, sk_jazz, STATE_TYPE);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  res = cmp_MMT_state(sk_c, sk_jazz);
  if (res) {
    printf("[!] Unit test failed.\nFunction:\tupdate_treehash (%u, %u).\n", s,
           t);
  }

  return res;
}

int test_MMT_update_treehash_node() {
  int res = 0;
  size_t j;
  uint32_t s, t;
  unsigned char sk_psd[PYR_N];
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;

  /* Set up randomized state in sk_c. */
  random_test_bytes(sk_c, MMT_BYTES);
  memcpy(sk_jazz, sk_c, MMT_BYTES);
  random_test_bytes(sk_psd, PYR_N);
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  t = 0;
  (state_c.treehash + t)->fin = 0;

  for (j = 0; j < (1ULL << PYR_TREE_HEIGHT); ++j) {
    s = j;
    (state_c.treehash + t)->idx = s;

    /* Final MMT nodes are put on the stack; clear it. */
    state_c.stack->size = 0;

    /* Sync struct changes to sk_jazz. */
    state_serialize(sk_jazz, &state_c, STATE_TYPE);

    update_treehash(sk_psd, t, &state_c, STATE_TYPE);
    MMT_update_treehash_jazz(sk_psd, t, sk_jazz, STATE_TYPE);

    state_serialize(sk_c, &state_c, STATE_TYPE);
    res = cmp_MMT_state(sk_c, sk_jazz);
    if (res) {
      printf("[!] Unit test failed.\nFunction:\tupdate_treehash (%u, %u).\n", s,
             t);
    }
  }

  return res;
}

int test_MMT_update_treehash_node_random(unsigned int nr_test) {
  int res = 0;
  size_t j;
  uint32_t s, t;
  unsigned char sk_psd[PYR_N];
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;

  /* Set up randomized state in sk_c. */
  random_test_bytes(sk_c, MMT_BYTES);
  memcpy(sk_jazz, sk_c, MMT_BYTES);
  random_test_bytes(sk_psd, PYR_N);
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  t = 0;
  (state_c.treehash + t)->fin = 0;
  state_c.stack->size = 0;

  for (j = 0; j < nr_test; ++j) {
    s = random_test_s(t);
    (state_c.treehash + t)->idx = s;

    /* Final MMT nodes are put on the stack; clear it. */
    state_c.stack->size = 0;

    /* Sync struct changes to sk_jazz. */
    state_serialize(sk_jazz, &state_c, STATE_TYPE);

    update_treehash(sk_psd, t, &state_c, STATE_TYPE);
    MMT_update_treehash_jazz(sk_psd, t, sk_jazz, STATE_TYPE);

    state_serialize(sk_c, &state_c, STATE_TYPE);
    res = cmp_MMT_state(sk_c, sk_jazz);
    if (res) {
      printf("[!] Unit test failed.\nFunction:\tupdate_treehash (%u, %u).\n", s,
             t);
    }
  }

  return res;
}

int test_MMT_update_treehash_tree_random(unsigned int max_height) {
  int res = 0;
  size_t j, k;
  uint32_t s, t;
  unsigned char sk_psd[PYR_N];
  unsigned char sk[MMT_BYTES];
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;

  /* Set up randomized state in sk. */
  random_test_bytes(sk, MMT_BYTES);
  random_test_bytes(sk_psd, PYR_N);

  for (j = 0; j < max_height && !res; ++j) {
    memcpy(sk_c, sk, MMT_BYTES);
    memcpy(sk_jazz, sk, MMT_BYTES);
    state_deserialize(sk_c, &state_c, STATE_TYPE);
    state_c.stack->size = 0;

    t = j;
    s = random_test_s(t);
    (state_c.treehash + t)->fin = 0;
    (state_c.treehash + t)->idx = s;

    /* Sync struct changes to sk_jazz. */
    state_serialize(sk_jazz, &state_c, STATE_TYPE);

    for (k = 0; k < (1ULL << j) && !res; ++k) {
      update_treehash(sk_psd, t, &state_c, STATE_TYPE);
      MMT_update_treehash_jazz(sk_psd, t, sk_jazz, STATE_TYPE);

      state_serialize(sk_c, &state_c, STATE_TYPE);
      res |= cmp_MMT_state(sk_c, sk_jazz);
    }

    if (res) {
      printf(
          "[!] Unit test failed.\nFunction:\tupdate_treehash (%u, %u), "
          "iteration %zu.\n",
          s, t, k);
    }
  }

  return res;
}

int main() {
  int res = 0;

#if 1
  res |= test_MMT_update_treehash_tree();
  res |= test_MMT_update_treehash_node();
#endif

  res |= test_MMT_update_treehash_node_random(NR_TEST);
  res |= test_MMT_update_treehash_tree_random(MAX_HEIGHT);

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

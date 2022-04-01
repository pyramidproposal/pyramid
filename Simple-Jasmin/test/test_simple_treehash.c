#include <stdint.h>
#include <stdio.h>

#include "../ref/params.h"
#include "../ref/pyr_simple.h"
#include "util/ABI_pack.h"
#include "util/misc.h"
#include "util/randomtestdata.h"

#define MAX_HEIGHT PYR_TREE_HEIGHT
#define NR_TEST 5000

int test_treehash_full_tree() {
  int res = 0;
  size_t i;
  uint32_t s, t;
  unsigned char next_seed_c[PYR_N];
  unsigned char next_seed_jazz[PYR_N];
  unsigned char sk_seed[PYR_N];
  unsigned char pub_seed[PYR_N];
  uint32_t subtree_addr[8];
  unsigned char root_c[PYR_N];
  unsigned char root_jazz[PYR_N];
  uint32_t other[2];

  random_test_bytes(sk_seed, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(subtree_addr, 8);
  s = 0;
  t = PYR_TREE_HEIGHT;
  set_other(other, s, t);

  tree_hash(root_c, next_seed_c, sk_seed, pub_seed, s, t, subtree_addr);
  treehash_jazz(root_jazz, next_seed_jazz, sk_seed, pub_seed, subtree_addr,
                other);

  for (i = 0; i < PYR_N; ++i) {
    if (root_c[i] != root_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\ttreehash (%u, "
          "%u).\nReason:\troot_c[%zu] != root_jazz[%zu] ==> %x != %x\n",
          s, t, i, i, root_c[i], root_jazz[i]);
    }
  }

  for (i = 0; i < PYR_N; ++i) {
    if (next_seed_c[i] != next_seed_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\ttreehash (%u, "
          "%u).\nReason:\tnext_seed_c[%zu] != next_seed_jazz[%zu] ==> %x != "
          "%x\n",
          s, t, i, i, next_seed_c[i], next_seed_jazz[i]);
    }
  }

  return res;
}

int test_treehash_node() {
  int res = 0;
  size_t i, j;
  uint32_t s, t;
  unsigned char next_seed_c[PYR_N];
  unsigned char next_seed_jazz[PYR_N];
  unsigned char sk_seed[PYR_N];
  unsigned char pub_seed[PYR_N];
  uint32_t leaf_idx;
  uint32_t subtree_addr[8];
  unsigned char root_c[PYR_N];
  unsigned char root_jazz[PYR_N];
  uint32_t other[2];

  random_test_bytes(sk_seed, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(&leaf_idx, 1);
  leaf_idx &= (1 << PYR_TREE_HEIGHT) - 1;
  random_test_32bit_blocks(subtree_addr, 8);
  t = 0;

  for (j = 0; j < (1 << PYR_TREE_HEIGHT); ++j) {
    s = j;
    set_other(other, s, t);

    tree_hash(root_c, next_seed_c, sk_seed, pub_seed, s, t, subtree_addr);
    treehash_jazz(root_jazz, next_seed_jazz, sk_seed, pub_seed, subtree_addr,
                  other);

    for (i = 0; i < PYR_N; ++i) {
      if (root_c[i] != root_jazz[i]) {
        res = -1;
        printf(
            "[!] Unit test failed.\nFunction:\ttreehash (%u, "
            "%u).\nReason:\troot_c[%zu] != root_jazz[%zu] ==> %x != %x\n",
            s, t, i, i, root_c[i], root_jazz[i]);
      }
    }

    for (i = 0; i < PYR_N; ++i) {
      if (next_seed_c[i] != next_seed_jazz[i]) {
        res = -1;
        printf(
            "[!] Unit test failed.\nFunction:\ttreehash (%u, "
            "%u).\nReason:\tnext_seed_c[%zu] != next_seed_jazz[%zu] ==> %x != "
            "%x\n",
            s, t, i, i, next_seed_c[i], next_seed_jazz[i]);
      }
    }
  }

  return res;
}

int test_treehash_node_random(unsigned int nr_test) {
  int res = 0;
  size_t i, j;
  uint32_t s, t;
  unsigned char next_seed_c[PYR_N];
  unsigned char next_seed_jazz[PYR_N];
  unsigned char sk_seed[PYR_N];
  unsigned char pub_seed[PYR_N];
  uint32_t subtree_addr[8];
  unsigned char root_c[PYR_N];
  unsigned char root_jazz[PYR_N];
  uint32_t other[2];

  random_test_bytes(sk_seed, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(subtree_addr, 8);
  t = 0;

  for (j = 0; j < nr_test; ++j) {
    s = random_test_s(t);
    set_other(other, s, t);

    tree_hash(root_c, next_seed_c, sk_seed, pub_seed, s, t, subtree_addr);
    treehash_jazz(root_jazz, next_seed_jazz, sk_seed, pub_seed, subtree_addr,
                  other);

    for (i = 0; i < PYR_N; ++i) {
      if (root_c[i] != root_jazz[i]) {
        res = -1;
        printf(
            "[!] Unit test failed.\nFunction:\ttreehash (%u, "
            "%u).\nReason:\troot_c[%zu] != root_jazz[%zu] ==> %x != %x\n",
            s, t, i, i, root_c[i], root_jazz[i]);
      }
    }

    for (i = 0; i < PYR_N; ++i) {
      if (next_seed_c[i] != next_seed_jazz[i]) {
        res = -1;
        printf(
            "[!] Unit test failed.\nFunction:\ttreehash (%u, "
            "%u).\nReason:\tnext_seed_c[%zu] != next_seed_jazz[%zu] ==> %x != "
            "%x\n",
            s, t, i, i, next_seed_c[i], next_seed_jazz[i]);
      }
    }
  }

  return res;
}

int test_treehash_tree_random(unsigned int max_height) {
  int res = 0;
  size_t i, j;
  uint32_t s, t;
  unsigned char next_seed_c[PYR_N];
  unsigned char next_seed_jazz[PYR_N];
  unsigned char sk_seed[PYR_N] = {0};
  unsigned char pub_seed[PYR_N] = {0};
  uint32_t subtree_addr[8] = {0};
  unsigned char root_c[PYR_N];
  unsigned char root_jazz[PYR_N];
  uint32_t other[2];

  /* random_test_bytes(sk_seed, PYR_N); */
  /* random_test_bytes(pub_seed, PYR_N); */
  /* random_test_32bit_blocks(subtree_addr, 8); */

  for (j = 0; j < max_height; ++j) {
    t = j;
    s = random_test_s(t);
    set_other(other, s, t);

    tree_hash(root_c, next_seed_c, sk_seed, pub_seed, s, t, subtree_addr);
    treehash_jazz(root_jazz, next_seed_jazz, sk_seed, pub_seed, subtree_addr,
                  other);

    for (i = 0; i < PYR_N; ++i) {
      if (root_c[i] != root_jazz[i]) {
        res = -1;
        printf(
            "[!] Unit test failed.\nFunction:\ttreehash (%u, "
            "%u).\nReason:\troot_c[%zu] != root_jazz[%zu] ==> %x != %x\n",
            s, t, i, i, root_c[i], root_jazz[i]);
      }
    }

    for (i = 0; i < PYR_N; ++i) {
      if (next_seed_c[i] != next_seed_jazz[i]) {
        res = -1;
        printf(
            "[!] Unit test failed.\nFunction:\ttreehash (%u, "
            "%u).\nReason:\tnext_seed_c[%zu] != next_seed_jazz[%zu] ==> %x != "
            "%x\n",
            s, t, i, i, next_seed_c[i], next_seed_jazz[i]);
      }
    }
  }

  return res;
}

int main() {
  int res = 0;

  /* Skipped. */
  /* #if 1 */
  res |= test_treehash_full_tree();
  /*   res |= test_treehash_node(); */
  /* #endif */

  /*   res |= test_treehash_node_random(NR_TEST); */
  /*   res |= test_treehash_tree_random(MAX_HEIGHT); */

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

#include <stdint.h>
#include <stdio.h>

#include "../ref/merkle.h"
#include "../ref/params.h"
#include "util/ABI_pack.h"
#include "util/misc.h"
#include "util/randomtestdata.h"

#define MAX_HEIGHT PYR_TREE_HEIGHT
#define NR_TEST 5000

int test_compute_root_full_tree() {
  int res = 0;
  size_t i;
  uint32_t s, t;
  unsigned char leaf[PYR_N];
  unsigned char auth_path[PYR_D * PYR_N];
  unsigned char pub_seed[PYR_N];
  unsigned char output_c[PYR_N];
  unsigned char output_jazz[PYR_N];
  uint32_t subtree_addr[8];
  uint32_t other[2];

  random_test_bytes(leaf, PYR_N);
  random_test_bytes(auth_path, PYR_D * PYR_N);
  random_test_32bit_blocks(subtree_addr, 8);
  s = 0;
  t = PYR_TREE_HEIGHT;
  set_other(other, s, t);

  compute_root(output_c, leaf, s, auth_path, t, pub_seed, subtree_addr);
  compute_root_jazz(output_jazz, leaf, auth_path, pub_seed, subtree_addr,
                    other);

  for (i = 0; i < PYR_N; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tcompute_root (%u, "
          "%u).\nReason:\toutput_c[%zu] != output_jazz[%zu] ==> %d != %d\n",
          s, t, i, i, output_c[i], output_jazz[i]);
    }
  }

  return res;
}

int test_compute_root_lowest_random(unsigned int nr_test) {
  int res = 0;
  size_t i, j;
  uint32_t s, t;
  unsigned char leaf[PYR_N];
  unsigned char auth_path[PYR_D * PYR_N];
  unsigned char pub_seed[PYR_N];
  unsigned char output_c[PYR_N];
  unsigned char output_jazz[PYR_N];
  uint32_t subtree_addr[8];
  uint32_t other[2];

  random_test_bytes(leaf, PYR_N);
  random_test_bytes(auth_path, PYR_D * PYR_N);
  random_test_32bit_blocks(subtree_addr, 8);
  t = 1;

  for (j = 0; j < nr_test && res; ++j) {
    s = random_test_s(t);
    set_other(other, s, t);

    compute_root(output_c, leaf, s, auth_path, t, pub_seed, subtree_addr);
    compute_root_jazz(output_jazz, leaf, auth_path, pub_seed, subtree_addr,
                      other);

    for (i = 0; i < PYR_N; ++i) {
      if (output_c[i] != output_jazz[i]) {
        res = -1;
        printf(
            "[!] Unit test failed.\nFunction:\tcompute_root (%u, "
            "%u).\nReason:\toutput_c[%zu] != output_jazz[%zu] ==> %d != %d\n",
            s, t, i, i, output_c[i], output_jazz[i]);
      }
    }
  }

  return res;
}

int test_compute_root_tree_random(unsigned int max_height) {
  int res = 0;
  size_t i, j;
  uint32_t s, t;
  unsigned char leaf[PYR_N];
  unsigned char auth_path[PYR_D * PYR_N];
  unsigned char pub_seed[PYR_N];
  unsigned char output_c[PYR_N];
  unsigned char output_jazz[PYR_N];
  uint32_t subtree_addr[8];
  uint32_t other[2];

  random_test_bytes(leaf, PYR_N);
  random_test_bytes(auth_path, PYR_D * PYR_N);
  random_test_32bit_blocks(subtree_addr, 8);

  for (j = 1; j < max_height && res; ++j) {
    t = j;
    s = random_test_s(t);
    set_other(other, s, t);

    compute_root(output_c, leaf, s, auth_path, t, pub_seed, subtree_addr);
    compute_root_jazz(output_jazz, leaf, auth_path, pub_seed, subtree_addr,
                      other);

    for (i = 0; i < PYR_N; ++i) {
      if (output_c[i] != output_jazz[i]) {
        res = -1;
        printf(
            "[!] Unit test failed.\nFunction:\tcompute_root (%u, "
            "%u).\nReason:\toutput_c[%zu] != output_jazz[%zu] ==> %d != %d\n",
            s, t, i, i, output_c[i], output_jazz[i]);
      }
    }
  }

  return res;
}

int main() {
  int res = 0;

  /* Skipped. */
#if 0
  res |= test_compute_root_full_tree();
#endif
  res |= test_compute_root_lowest_random(NR_TEST);
  res |= test_compute_root_tree_random(MAX_HEIGHT);

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

#include <stdint.h>
#include <stdio.h>

#include "../ref/params.h"
#include "../ref/treehash.h"
#include "util/randomtestdata.h"

int main() {
  int i, j;
  unsigned char sk_seed[PYR_N];
  unsigned char pub_seed[PYR_N];
  uint32_t leaf_idx;
  uint32_t subtree_addr[8];
  unsigned char root_c[PYR_N];
  unsigned char root_jazz[PYR_N];
  unsigned char auth_path_c[PYR_N * PYR_D];
  unsigned char auth_path_jazz[PYR_N * PYR_D];

  random_test_bytes(sk_seed, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(&leaf_idx, 1);
  leaf_idx &= (1 << PYR_TREE_HEIGHT) - 1; /* ensure within tree */
  random_test_32bit_blocks(subtree_addr, 8);

  for (size_t k = 0; k < PYR_N * PYR_D; k++) {
    auth_path_jazz[k] = 0;
  }

  // Test
  treehash(root_c, auth_path_c, sk_seed, pub_seed, leaf_idx, subtree_addr);
  treehash_jazz(root_jazz, auth_path_jazz, sk_seed, pub_seed, leaf_idx,
                subtree_addr);

  for (i = 0; i < PYR_N; ++i) {
    if (root_c[i] != root_jazz[i]) {
      printf(
          "[!] Unit test failed.\nFunction:\ttreehash.\nReason:\troot_c[%d] != "
          "root_jazz[%d] ==> %d != %d\n",
          i, i, root_c[i], root_jazz[i]);
    }
  }
  for (j = 0; j < PYR_D; ++j) {
    for (i = 0; i < PYR_N; ++i) {
      if (auth_path_c[i + j * PYR_N] != auth_path_jazz[i + j * PYR_N]) {
        printf(
            "[!] Unit test "
            "failed.\nFunction:\ttreehash.\nReason:\tauth_path_c[%d][%d] != "
            "auth_path_jazz[%d][%d] ==> %d != %d\n",
            j, i, j, i, auth_path_c[i + j * PYR_N],
            auth_path_jazz[i + j * PYR_N]);
      }
    }
  }

  return 0;
}

#include <stdint.h>
#include <stdio.h>

#include "../ref/params.h"
#include "../ref/xmss_simple.h"
#include "util/randomtestdata.h"

int test_tree_sig() {
  int res = 0;
  size_t i;
  unsigned char sig_c[PYR_XMSS_BYTES];
  unsigned char sig_jazz[PYR_XMSS_BYTES];
  unsigned char M[PYR_N];
  unsigned char sk[PYR_SK_CORE_BYTES];
  const unsigned char *sk_ssd = sk + SK_SSD;
  const unsigned char *sk_psd = sk + SK_PSD;
  uint32_t idx_leaf;
  uint32_t subtree_addr[8];
  unsigned char auth_path[PYR_N * PYR_TREE_HEIGHT];

  random_test_bytes(M, PYR_N);
  random_test_bytes(sk, PYR_SK_CORE_BYTES);
  random_test_32bit_blocks(subtree_addr, 8);
  random_test_32bit_blocks(&idx_leaf, 1);
  idx_leaf &= (1 << PYR_TREE_HEIGHT) - 1;

  tree_sig(sig_c, M, sk_ssd, sk_psd, idx_leaf, subtree_addr, auth_path);
  tree_sig_jazz(sig_jazz, M, sk, idx_leaf, subtree_addr, auth_path);

  for (i = 0; i < PYR_XMSS_BYTES; ++i) {
    if (sig_c[i] != sig_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\ttree_sig "
          "(0).\nReason:\tsig_c[%zu] != sig_jazz[%zu] ==> %d != %d\n",
          i, i, sig_c[i], sig_jazz[i]);
    }
  }

  return res;
}

int main() {
  int res = 0;

  res |= test_tree_sig();

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

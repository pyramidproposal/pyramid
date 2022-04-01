#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ref/fips202.h"
#include "../ref/hash.h"
#include "../ref/params.h"
#include "util/randomtestdata.h"

int test_prf() {
  int res = 0;
  size_t i;
  unsigned char out_c[PYR_N];
  unsigned char out_jazz[PYR_N];

  unsigned char key[PYR_N];
  unsigned char pub_seed[PYR_N];
  uint32_t addr[8];

  random_test_bytes(key, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(addr, 8);

  prf(out_c, key, pub_seed, addr);
  prf_jazz(out_jazz, key, pub_seed, addr);

  for (i = 0; i < PYR_N; ++i) {
    if (out_c[i] != out_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tprf.\nReason:\tout_c[%zu] != "
          "out_jazz[%zu] ==> %hhu != %hhu\n",
          i, i, out_c[i], out_jazz[i]);
    }
  }
  return res;
}

int test_h_msg(size_t mlen) {
  int res = 0;
  size_t i;
  unsigned char out_c[PYR_N];
  unsigned char out_jazz[PYR_N];

  unsigned char *m;
  unsigned char R[PYR_N] = {0};
  unsigned char root[PYR_N] = {0};
  uint32_t addr[8];

  m = (unsigned char *)malloc(mlen);

  random_test_bytes(R, PYR_N);
  random_test_bytes(root, PYR_N);
  random_test_32bit_blocks(addr, 8);
  random_test_bytes(m, mlen);

  h_msg(out_c, R, root, addr, m, mlen);
  h_msg_jazz(out_jazz, R, root, addr, m, mlen);

  free(m);

  for (i = 0; i < PYR_N; ++i) {
    if (out_c[i] != out_jazz[i]) {
      printf(
          "[!] Unit test failed.\nFunction:\th_msg.\nReason:\tout_c[%zu] != "
          "out_jazz[%zu] ==> %hhu != %hhu\n",
          i, i, out_c[i], out_jazz[i]);
    }
  }

  return res;
}

int main() {
  int res = 0;
  size_t i;

  res |= test_prf();

  for (i = 0; i < 2 * SHAKE256_RATE + 10; ++i) {
    res |= test_h_msg(i);
  }

  return 0;
}

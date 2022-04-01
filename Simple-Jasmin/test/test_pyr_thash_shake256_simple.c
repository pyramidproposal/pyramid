#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/fips202.h"
#include "../ref/params.h"
#include "../ref/thash.h"
#include "util/randomtestdata.h"

int test_thash1() {
  int res = 0;
  size_t i;
  unsigned char in_c[PYR_N];
  unsigned char out_c[PYR_N];
  unsigned char inout_jazz[PYR_N];

  unsigned char pub_seed[PYR_N];
  uint32_t addr[8];

  random_test_bytes(pub_seed, PYR_N);
  random_test_bytes(in_c, PYR_N);
  memcpy(inout_jazz, in_c, PYR_N);
  random_test_32bit_blocks(addr, 8);

  thash(out_c, in_c, 1, pub_seed, addr);
  thash1_jazz(inout_jazz, pub_seed, addr);

  for (i = 0; i < PYR_N; ++i) {
    if (out_c[i] != inout_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tthash1.\nReason:\tout_c[%zu] != "
          "out_jazz[%zu] ==> %hhu != %hhu\n",
          i, i, out_c[i], inout_jazz[i]);
    }
  }

  return res;
}

int test_thash2() {
  int res = 0;
  size_t i;
  unsigned char in_c[2 * PYR_N];
  unsigned char out_c[PYR_N];
  unsigned char inout_jazz[2 * PYR_N];

  unsigned char pub_seed[PYR_N];
  uint32_t addr[8];

  random_test_bytes(in_c, 2 * PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  memcpy(inout_jazz, in_c, 2 * PYR_N);
  random_test_32bit_blocks(addr, 8);

  // Test
  thash(out_c, in_c, 2, pub_seed, addr);
  thash2_jazz(inout_jazz, pub_seed, addr);

  /* The right side of the Jasmin buffer should be ignored. */
  for (i = 0; i < PYR_N; ++i) {
    if (out_c[i] != inout_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tthash2.\nReason:\tout_c[%zu] != "
          "inout_jazz[%zu] ==> %hhu != %hhu\n",
          i, i, out_c[i], inout_jazz[i]);
    }
  }

  return res;
}

int test_thashLEN() {
  int res = 0;
  size_t i;
  unsigned char in_c[PYR_WOTS_BYTES];
  unsigned char in_jazz[PYR_WOTS_BYTES];
  unsigned char out_c[PYR_N];
  unsigned char out_jazz[PYR_N];

  unsigned char pub_seed[PYR_N];
  uint32_t addr[8];

  random_test_bytes(in_c, PYR_WOTS_BYTES);
  random_test_bytes(pub_seed, PYR_N);
  memcpy(in_jazz, in_c, PYR_WOTS_BYTES);
  random_test_32bit_blocks(addr, 8);

  // Test
  thash(out_c, in_c, PYR_WOTS_LEN, pub_seed, addr);
  thashLEN_jazz(out_jazz, in_jazz, pub_seed, addr);

  for (i = 0; i < PYR_N; ++i) {
    if (out_c[i] != out_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tthashLEN.\nReason:\tout_c[%zu] != "
          "out_jazz[%zu] ==> %hhu != %hhu\n",
          i, i, out_c[i], out_jazz[i]);
    }
  }

  return res;
}

int main() {
  int res = 0;

  res |= test_thash1();
  res |= test_thash2();
  res |= test_thashLEN();

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

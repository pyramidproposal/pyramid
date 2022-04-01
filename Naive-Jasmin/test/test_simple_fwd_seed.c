#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/address.h"
#include "../ref/params.h"
#include "../ref/utils.h"
#include "util/randomtestdata.h"

int test_fwd_seed() {
  int res = 0;
  size_t i;
  uint32_t s;
  uint32_t addr[8] = {0};
  unsigned char seed_c[PYR_N];
  unsigned char seed_jazz[PYR_N];
  unsigned char pub_seed[PYR_N];

  random_test_bytes(seed_c, PYR_N);
  memcpy(seed_jazz, seed_c, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  s = (1 << (PYR_TREE_HEIGHT - 1));

  /* s is inline int argument in jasmin */
  fwd_seed(seed_c, seed_c, pub_seed, s, addr);
  fwd_seed_jazz(seed_jazz, pub_seed, addr);

  for (i = 0; i < PYR_N; ++i) {
    if (seed_c[i] != seed_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tfwd_seed.\nReason:\tseed_c[%zu] "
          "!= seed_jazz[%zu] ==> %x != %x\n",
          i, i, seed_c[i], seed_jazz[i]);
    }
  }

  return res;
}

int main() {
  int res = 0;

  res = test_fwd_seed();

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

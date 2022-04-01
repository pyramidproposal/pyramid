#include <stdint.h>
#include <stdio.h>

#include "../ref/fips202.h"
#include "util/randomtestdata.h"

int main() {
  // Loop variable
  int i;

  // Declare variables for test
  unsigned char h_c[SHA3_256_RATE];
  unsigned char h_jazz[SHA3_256_RATE];
  uint64_t state_c[25];
  uint64_t state_jazz[25];

  // Initialize variables for test
  random_test_bytes(h_c, SHA3_256_RATE);
  random_test_64bit_blocks(state_c, 25);

  for (i = 0; i < SHA3_256_RATE; ++i) {
    h_jazz[i] = h_c[i];
  }

  for (i = 0; i < 25; ++i) {
    state_jazz[i] = state_c[i];
  }

  // Test
  keccak_squeezeblocks(h_c, 1, state_c, SHA3_256_RATE);
  keccak_squeezeblocks_256_256_jazz(h_jazz, state_jazz);

  for (i = 0; i < SHA3_256_RATE; ++i) {
    if (h_c[i] != h_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tkeccak_squeezeblocks_256_256.\nReason:\th_c[%d] "
          "!= h_jazz[%d] ==> %d != %d\n",
          i, i, h_c[i], h_jazz[i]);
    }
  }

  for (i = 0; i < 25; ++i) {
    if (state_c[i] != state_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tkeccak_squeezeblocks_256_256.\nReason:\tstate_c["
          "%d] != state_jazz[%d] ==> %ld != %ld\n",
          i, i, state_c[i], state_jazz[i]);
    }
  }

  return 0;
}

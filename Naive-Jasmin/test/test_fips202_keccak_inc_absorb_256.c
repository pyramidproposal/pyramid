#include <stdint.h>
#include <stdio.h>

#include "../ref/fips202.h"
#include "util/randomtestdata.h"

#define INLEN (2 * SHAKE256_RATE + 10)

int main() {
  // Loop variable
  int i;

  // Declare variables for test
  uint8_t input[INLEN];
  uint64_t s_inc_c[26];
  uint64_t s_inc_jazz[26];

  // Initialize variables for test
  random_test_bytes(input, INLEN);

  // Test
  shake256_inc_init(s_inc_c);
  shake256_inc_absorb(s_inc_c, input, INLEN);

  keccak_inc_init_jazz(s_inc_jazz);
  keccak_inc_absorb_256_jazz(s_inc_jazz, input, INLEN);

  for (i = 0; i < 26; ++i) {
    if (s_inc_c[i] != s_inc_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tshake256_PYR_N_INLEN.\nReason:\ts_inc_c[%d] != "
          "s_inc_jazz[%d] ==> %lu != %lu\n",
          i, i, s_inc_c[i], s_inc_jazz[i]);
    }
  }

  return 0;
}

#include <stdint.h>
#include <stdio.h>

#include "../ref/fips202.h"
#include "../ref/params.h"
#include "util/jasmin_constants.h"
#include "util/randomtestdata.h"

int main() {
  // Loop variable
  int i;

  // Declare variables for test
  uint8_t input[HMSG_CONST_INLEN_C];
  uint64_t s_inc_c[26];
  uint64_t s_inc_jazz[26];

  // Initialize variables for test
  random_test_bytes(input, HMSG_CONST_INLEN_C);

  // Test
  shake256_inc_init(s_inc_c);
  shake256_inc_absorb(s_inc_c, input, HMSG_CONST_INLEN_C);

  keccak_inc_init_jazz(s_inc_jazz);
  keccak_inc_absorb_256_HMSG_CONST_INLEN_jazz(s_inc_jazz, input);

  for (i = 0; i < 26; ++i) {
    if (s_inc_c[i] != s_inc_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tkeccak_inc_absorb_256_HMSG_CONST_INLEN.\nReason:"
          "\ts_inc_c[%d] != s_inc_c[%d] ==> %lu != %lu\n",
          i, i, s_inc_c[i], s_inc_jazz[i]);
    }
  }

  return 0;
}

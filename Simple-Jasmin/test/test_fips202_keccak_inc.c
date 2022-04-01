#include <stdint.h>
#include <stdio.h>

#include "../ref/fips202.h"
#include "../ref/params.h"
#include "util/jasmin_constants.h"
#include "util/randomtestdata.h"

#define INLEN (2 * SHAKE256_RATE + 10)

int main() {
  // Loop variable
  int i;

  // Declare variables for test
  uint8_t input[HMSG_CONST_INLEN_C];
  uint8_t msg[INLEN];
  uint64_t s_inc_c[26];
  uint64_t s_inc_jazz[26];
  uint8_t output_c[PYR_N];
  uint8_t output_jazz[PYR_N];

  // Initialize variables for test
  random_test_bytes(input, HMSG_CONST_INLEN_C);

  // Test
  shake256_inc_init(s_inc_c);
  keccak_inc_init_jazz(s_inc_jazz);

  for (i = 0; i < 26; ++i) {
    if (s_inc_c[i] != s_inc_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tkeccak_inc_init.\nReason:\ts_inc_c[%d] != "
          "s_inc_c[%d] ==> %lu != %lu\n",
          i, i, s_inc_c[i], s_inc_jazz[i]);
    }
  }

  shake256_inc_absorb(s_inc_c, input, HMSG_CONST_INLEN_C);
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

  shake256_inc_absorb(s_inc_c, msg, INLEN);
  keccak_inc_absorb_256_jazz(s_inc_jazz, msg, INLEN);

  for (i = 0; i < 26; ++i) {
    if (s_inc_c[i] != s_inc_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tkeccak_inc_absorb_256.\nReason:\ts_inc_c[%d] != "
          "s_inc_c[%d] ==> %lu != %lu\n",
          i, i, s_inc_c[i], s_inc_jazz[i]);
    }
  }

  shake256_inc_finalize(s_inc_c);
  keccak_inc_finalize_256_jazz(s_inc_jazz);

  for (i = 0; i < 26; ++i) {
    if (s_inc_c[i] != s_inc_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tkeccak_inc_finalize_256.\nReason:\ts_inc_c[%d] "
          "!= s_inc_c[%d] ==> %lu != %lu\n",
          i, i, s_inc_c[i], s_inc_jazz[i]);
    }
  }

  shake256_inc_squeeze(output_c, PYR_N, s_inc_c);
  keccak_inc_squeeze_256_PYR_N_jazz(output_jazz, s_inc_jazz);

  for (i = 0; i < PYR_N; ++i) {
    if (output_c[i] != output_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tkeccak_inc_squeeze_256_PYR_N.\nReason:\ts_inc_c["
          "%d] != s_inc_c[%d] ==> %hhu != %hhu\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }

  return 0;
}

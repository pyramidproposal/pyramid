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
  unsigned char input[THLEN_INLEN];
  unsigned char output_c[PYR_N];
  unsigned char output_jazz[PYR_N];

  // Initialize variables for test
  random_test_bytes(input, THLEN_INLEN);

  // Test

  shake256(output_c, PYR_N, input, THLEN_INLEN);
  shake256_PYR_N_THLEN_INLEN_jazz(output_jazz, input);

  for (i = 0; i < PYR_N; ++i) {
    if (output_c[i] != output_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tshake256_PYR_N_THLEN_INLEN.\nReason:\toutput_c[%"
          "d] != output_jazz[%d] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }

  return 0;
}

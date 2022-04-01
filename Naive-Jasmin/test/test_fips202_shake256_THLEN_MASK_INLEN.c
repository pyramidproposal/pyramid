#include <stdint.h>
#include <stdio.h>

#include "../ref/fips202.h"
#include "util/jasmin_constants.h"
#include "util/randomtestdata.h"

int main() {
  // Loop variable
  int i;

  // Declare variables for test
  unsigned char input[MASK_INLEN];
  unsigned char output_c[PYR_WOTS_BYTES];
  unsigned char output_jazz[PYR_WOTS_BYTES];

  // Initialize variables for test
  random_test_bytes(input, MASK_INLEN);

  // Test
  shake256(output_c, PYR_WOTS_BYTES, input, MASK_INLEN);
  shake256_THLEN_MASK_INLEN_jazz(output_jazz, input);

  for (i = 0; i < PYR_WOTS_BYTES; ++i) {
    if (output_c[i] != output_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tshake256_THLEN_MASK_INLEN.\nReason:\toutput_c[%"
          "d] != output_jazz[%d] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }

  return 0;
}

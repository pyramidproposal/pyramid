#include <stdint.h>
#include <stdio.h>

#include "../ref/fips202.h"
#include "util/randomtestdata.h"

int main() {
  // Declare variables for test
  unsigned char x[8];
  uint64_t res_c;
  uint64_t res_jazz;

  // Initialize variables for test
  random_test_bytes(x, 8);

  // Test
  res_c = load64(x);
  res_jazz = load64_jazz(x);

  if (res_c != res_jazz) {
    printf(
        "[!] Unit test failed.\nFunction:\tload64.\nReason:\tres_c != res_jazz "
        "==> %ld != %ld\n",
        res_c, res_jazz);
  }

  return 0;
}

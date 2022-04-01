#include <stdint.h>
#include <stdio.h>

#include "../ref/fips202.h"
#include "util/randomtestdata.h"

int main() {
  // Loop variable
  int i;

  // Declare variables for test
  uint64_t state_c[25];
  uint64_t state_jazz[25];

  // Initialize variables for test
  random_test_64bit_blocks(state_c, 25);

  for (i = 0; i < 25; ++i) {
    state_jazz[i] = state_c[i];
  }

  // Test
  KeccakF1600_StatePermute(state_c);
  KeccakF1600_StatePermute_jazz(state_jazz);

  for (i = 0; i < 25; ++i) {
    if (state_c[i] != state_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tKeccakF1600_StatePermute.\nReason:\tstate_c[%d] "
          "!= state_jazz[%d] ==> %ld != %ld\n",
          i, i, state_c[i], state_jazz[i]);
    }
  }

  return 0;
}

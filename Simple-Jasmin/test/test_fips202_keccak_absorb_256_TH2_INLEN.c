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
  unsigned char m_c[TH2_INLEN];
  unsigned char m_jazz[TH2_INLEN];

  /* The SPHINCS+ reference implementation zeroes the state. */
  uint64_t state_c[25] = {0};
  uint64_t state_jazz[25] = {0};

  // Initialize variables for test
  random_test_bytes(m_c, TH2_INLEN);

  for (i = 0; i < TH2_INLEN; ++i) {
    m_jazz[i] = m_c[i];
  }

  // Test
  keccak_absorb(state_c, SHAKE256_RATE, m_c, TH2_INLEN, 0x1F);
  keccak_absorb_256_TH2_INLEN_jazz(state_jazz, m_jazz);

  for (i = 0; i < 25; ++i) {
    if (state_c[i] != state_jazz[i]) {
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tkeccak_absorb_256_TH2_INLEN.\nReason:\tstate_c[%"
          "d] != state_jazz[%d] ==> %ld != %ld\n",
          i, i, state_c[i], state_jazz[i]);
    }
  }

  return 0;
}

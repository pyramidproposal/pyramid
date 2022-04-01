#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../api.h"
#include "params.h"
#include "randombytes.h"

#define PYR_MLEN 32
#define PYR_SIGNATURES ((LAST_VALID_IDX + 1 < 5000) ? LAST_VALID_IDX + 1 : 5000)

int main() {
  int ret = 0;
  uint64_t i, j;

  /* Make stdout buffer more responsive. */
  setbuf(stdout, NULL);

  // Adapted the testing interface to use the API as much as possible.
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char *m = malloc(PYR_MLEN);
  unsigned char *sm = malloc(CRYPTO_BYTES + PYR_MLEN);
  unsigned char *mout = malloc(CRYPTO_BYTES + PYR_MLEN);
  unsigned long long smlen;

  randombytes(m, PYR_MLEN);

  printf("Dumping signatures for FS-SIMPLE\n");

  if (crypto_sign_keypair(pk, sk)) {
    printf("failed!\n");
    return -1;
  }
  printf("successful.\n");

  printf("Testing %lu signatures.. \n", (uint64_t)PYR_SIGNATURES);

  for (i = 0; (i < PYR_SIGNATURES) && !ret; i++) {
    printf("  - iteration #%lu:\n", i);

    crypto_sign(sm, &smlen, m, PYR_MLEN, sk);

    for (j = 0; j < CRYPTO_BYTES; j++) {
      printf("%x", sm[j]);
    }
    printf("\n");
  }

  printf("Dumped %lu signatures for FS-SIMPLE\n", i);

  free(m);
  free(sm);
  free(mout);

  return ret;
}

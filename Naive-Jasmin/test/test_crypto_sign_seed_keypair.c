#include <stdint.h>
#include <stdio.h>

#include "../ref/api.h"
#include "../ref/format.h"
#include "../ref/params.h"
#include "util/compare.h"
#include "util/randomtestdata.h"

int main() {
  int res = 0;
  unsigned char seed[CRYPTO_SEEDBYTES];
  unsigned char pk_c[PYR_PK_BYTES];
  unsigned char pk_jazz[PYR_PK_BYTES];
  unsigned char sk_c[CRYPTO_SECRETKEYBYTES];
  unsigned char sk_jazz[CRYPTO_SECRETKEYBYTES];
  int retval_c, retval_jazz;

  random_test_bytes(seed, CRYPTO_SEEDBYTES);

  retval_c = crypto_sign_seed_keypair(pk_c, sk_c, seed);
  retval_jazz = crypto_sign_seed_keypair_jazz(pk_jazz, sk_jazz, seed);

  if (retval_c != retval_jazz) {
    res = -1;
    printf(
        "[!] Unit test "
        "failed.\nFunction:\tcrypto_sign_seed_keypair.\nReason:\tretval_c != "
        "retval_jazz ==> %d != %d\n",
        retval_c, retval_jazz);
  }

  if (!res) {
    res |= cmp_SK(sk_c, sk_jazz);
    res |= cmp_PK(pk_c, pk_jazz);
  }

  if (res) {
    printf("[!] Unit test failed.\nFunction:\tcrypto_sign_seed_keypair.\n");
  }

  return 0;
}

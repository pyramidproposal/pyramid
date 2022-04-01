#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "../ref/api.h"
#include "string.h"
#include "util/compare.h"
#include "util/misc.h"
#include "util/randomtestdata.h"

/* We try 5 invalid signings. */
#define NR_TEST                                                             \
  (((CRYPTO_MAX_VALID_IDX + 1 + 5) < 5000) ? (CRYPTO_MAX_VALID_IDX + 1 + 5) \
                                           : 5000)
#define MAX_LEN 2000

int test_crypto_sign_signature() {
  int res = 0;
  int status = 0;

  size_t i;
  size_t mlen, siglen_c, siglen_jazz;
  unsigned char m[MAX_LEN - 1];
  unsigned char sig_c[PYR_BYTES];
  unsigned char sig_jazz[PYR_BYTES];
  unsigned char pk[PYR_PK_BYTES];
  unsigned char sk_c[CRYPTO_SECRETKEYBYTES];
  unsigned char sk_jazz[CRYPTO_SECRETKEYBYTES];
  int retval_c, retval_jazz;

  crypto_sign_keypair(pk, sk_c);
  memcpy(sk_jazz, sk_c, CRYPTO_SECRETKEYBYTES);

  for (i = 0; i < NR_TEST && res == 0; ++i) {
    printf("Iteration %zu ", i);
    if (i == CRYPTO_MAX_VALID_IDX) {
      printf("(should delete sk)\n");
    } else if (i > CRYPTO_MAX_VALID_IDX) {
      printf("(should skip signature)\n");
    } else {
      printf("\n");
    }

    random_test_64bit_blocks(&mlen, 1);
    mlen = mlen % MAX_LEN;
    random_test_bytes(m, mlen);

    retval_c = crypto_sign_signature(sig_c, &siglen_c, m, mlen, sk_c);
    retval_jazz =
        crypto_sign_signature_jazz(sig_jazz, &siglen_jazz, m, mlen, sk_jazz);

    if (retval_c != retval_jazz) {
      status = -1;
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tcrypto_sign_signature.\nReason:\tretval_c != "
          "retval_jazz ==> %d != %d\n",
          retval_c, retval_jazz);
    }

    if (siglen_c != siglen_jazz) {
      status = -2;
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tcrypto_sign_signature.\nReason:\tsiglen_c != "
          "siglen_jazz ==> %zu != %zu\n",
          siglen_c, siglen_jazz);
    }

    switch (status) {
      case 0:
        if (retval_jazz != -2) {
          /* Comparison is skipped because the signature output is not defined
           * for invalid idx. */
          res |= cmp_SIG(sig_c, sig_jazz);
        }
        /* The sk output is always defined. */
        res |= cmp_SK(sk_c, sk_jazz);
        break;

      case -1:
        printf("Comparison skipped due to wrong return value. ");
        break;

      case -2:
        printf("Comparison skipped due to wrong signature length. ");
        break;
    }
  }

  if (res) {
    printf(
        "[!] Unit test failed.\nFunction:\tcrypto_sign_signature. Index: "
        "%zu.\n",
        i);
  }

  return res;
}

int test_crypto_sign_signature_invalid_sk() {
  int res = 0;

  size_t i;
  size_t mlen, siglen_c, siglen_jazz;
  unsigned char m[MAX_LEN - 1];
  unsigned char sig_c[PYR_BYTES];
  unsigned char sig_jazz[PYR_BYTES];
  unsigned char sk_c[CRYPTO_SECRETKEYBYTES];
  unsigned char sk_jazz[CRYPTO_SECRETKEYBYTES];
  int retval_c, retval_jazz;

  deleted_sk(sk_c);
  memcpy(sk_jazz, sk_c, CRYPTO_SECRETKEYBYTES);

  random_test_64bit_blocks(&mlen, 1);
  mlen = mlen % MAX_LEN;
  random_test_bytes(m, mlen);

  retval_c = crypto_sign_signature(sig_c, &siglen_c, m, mlen, sk_c);
  retval_jazz =
      crypto_sign_signature_jazz(sig_jazz, &siglen_jazz, m, mlen, sk_jazz);

  if (retval_c != retval_jazz) {
    res = -1;
    printf(
        "[!] Unit test failed.\nFunction:\tcrypto_sign_signature "
        "(invalid_sk).\nReason:\tretval_c != retval_jazz ==> %d != %d\n",
        retval_c, retval_jazz);
  }

  if (siglen_c != siglen_jazz) {
    res = -1;
    printf(
        "[!] Unit test failed.\nFunction:\tcrypto_sign_signature "
        "(invalid_sk).\nReason:\tsiglen_c != siglen_jazz ==> %zu != %zu\n",
        siglen_c, siglen_jazz);
  }

  if (res) {
    printf(
        "[!] Unit test failed.\nFunction:\tcrypto_sign_signature (invalid_sk). "
        "Index: %zu.\n",
        i);
  }

  return res;
}

int main() {
  int res = 0;

  /* We note that this test does not necessarily reach the final index for every
     parameter set. We currently only represent "final index" cases by testing
     parameter sets that have a small PYR_TREE_HEIGHT. The alternative
     solution is to build an artificial sk for testing (including a (semi-)valid
     external state).
  */
  res |= test_crypto_sign_signature();
  res |= test_crypto_sign_signature_invalid_sk();

  return 0;
}

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/api.h"
#include "../ref/format.h"
#include "../ref/params.h"
#include "util/randomtestdata.h"

#define NR_TEST                                                             \
  (((CRYPTO_MAX_VALID_IDX + 1 + 5) < 5000) ? (CRYPTO_MAX_VALID_IDX + 1 + 5) \
                                           : 5000)
#define MAX_LEN 2000

int test_regular(const unsigned char *sig, size_t siglen,
                 const unsigned char *m, size_t mlen, const unsigned char *pk) {
  int res = 0;
  int retval_c, retval_jazz;

  retval_c = crypto_sign_verify(sig, siglen, m, mlen, pk);
  retval_jazz = crypto_sign_verify_jazz(sig, siglen, m, mlen, pk);

  if (retval_c != retval_jazz) {
    res = -1;
    printf(
        "[!] Unit test failed.\nFunction:\tcrypto_sign_verify "
        "(regular).\nReason:\tretval_c != retval_jazz ==> %d != %d\n",
        retval_c, retval_jazz);
  }

  return res;
}

int test_flip_IDX(const unsigned char *sig, size_t siglen,
                  const unsigned char *m, size_t mlen,
                  const unsigned char *pk) {
  int res = 0;
  int retval_c, retval_jazz;
  unsigned char sig_IDX[PYR_BYTES];

  memcpy(sig_IDX, sig, PYR_BYTES);
  sig_IDX[0] ^= 1;

  retval_c = crypto_sign_verify(sig_IDX, siglen, m, mlen, pk);
  retval_jazz = crypto_sign_verify_jazz(sig_IDX, siglen, m, mlen, pk);

  if (retval_c != retval_jazz) {
    printf(
        "[!] Unit test failed.\nFunction:\tcrypto_sign_verify "
        "(IDX).\nReason:\tretval_c != retval_jazz ==> %d != %d\n",
        retval_c, retval_jazz);
  }

  return res;
}

int test_flip_RND(const unsigned char *sig, size_t siglen,
                  const unsigned char *m, size_t mlen,
                  const unsigned char *pk) {
  int res = 0;
  int retval_c, retval_jazz;
  unsigned char sig_RND[PYR_BYTES];

  memcpy(sig_RND, sig, PYR_BYTES);
  sig_RND[PYR_IDX_BYTES] ^= 1;

  retval_c = crypto_sign_verify(sig_RND, siglen, m, mlen, pk);
  retval_jazz = crypto_sign_verify_jazz(sig_RND, siglen, m, mlen, pk);

  if (retval_c != retval_jazz) {
    printf(
        "[!] Unit test failed.\nFunction:\tcrypto_sign_verify "
        "(RND).\nReason:\tretval_c != retval_jazz ==> %d != %d\n",
        retval_c, retval_jazz);
  }

  return res;
}

int test_flip_HTS(const unsigned char *sig, size_t siglen,
                  const unsigned char *m, size_t mlen,
                  const unsigned char *pk) {
  int res = 0;
  int retval_c, retval_jazz;
  unsigned char sig_HTS[PYR_BYTES];

  memcpy(sig_HTS, sig, PYR_BYTES);
  sig_HTS[PYR_IDX_BYTES + PYR_N] ^= 1;

  retval_c = crypto_sign_verify(sig_HTS, siglen, m, mlen, pk);
  retval_jazz = crypto_sign_verify_jazz(sig_HTS, siglen, m, mlen, pk);

  if (retval_c != retval_jazz) {
    printf(
        "[!] Unit test failed.\nFunction:\tcrypto_sign_verify "
        "(HTS).\nReason:\tretval_c != retval_jazz ==> %d != %d\n",
        retval_c, retval_jazz);
  }

  return res;
}

int test_incorrect_len(const unsigned char *sig, const unsigned char *m,
                       size_t mlen, const unsigned char *pk) {
  int res = 0;
  size_t siglen;
  int retval_c, retval_jazz;

  random_test_64bit_blocks(&siglen, 1);

  retval_c = crypto_sign_verify(sig, siglen, m, mlen, pk);
  retval_jazz = crypto_sign_verify_jazz(sig, siglen, m, mlen, pk);

  if (retval_c != retval_jazz) {
    res = -1;
    printf(
        "[!] Unit test failed.\nFunction:\tcrypto_sign_verify (incorrect "
        "len).\nReason:\tretval_c != retval_jazz ==> %d != %d\n",
        retval_c, retval_jazz);
  }

  return res;
}

int main() {
  int res = 0;
  size_t i;
  unsigned char sig[PYR_BYTES];
  size_t siglen, mlen;
  unsigned char m[MAX_LEN];
  unsigned char pk[PYR_PK_BYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  int retval_c;

  random_test_64bit_blocks(&mlen, 1);
  mlen = mlen % MAX_LEN;
  random_test_bytes(m, mlen);

  retval_c = crypto_sign_keypair(pk, sk);
  assert(retval_c == 0);

  for (i = 0; i < NR_TEST && res == 0; ++i) {
    printf("Iteration %zu\n", i);

    retval_c = crypto_sign_signature(sig, &siglen, m, mlen, sk);

    res |= test_regular(sig, siglen, m, mlen, pk);
    res |= test_flip_IDX(sig, siglen, m, mlen, pk);
    res |= test_flip_HTS(sig, siglen, m, mlen, pk);
    res |= test_flip_RND(sig, siglen, m, mlen, pk);
    res |= test_incorrect_len(sig, m, mlen, pk);
  }

  if (res) {
    printf("Tests failed (%zu).\n", i);
  }

  return 0;
}

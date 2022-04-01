#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../../ref/format.h"
#include "../../ref/params.h"

int cmp_SK_CORE(const unsigned char *sk_c, const unsigned char *sk_jazz) {
  int res = 0;
  size_t offset, i;
  offset = 0;

  for (i = 0; i < PYR_IDX_BYTES; ++i) {
    if (sk_c[offset + i] != sk_jazz[offset + i]) {
      res = -1;
      printf("[!] SK_IDX: sk_c[%zu] != sk_jazz[%zu] ==> %x != %x\n", i, i,
             sk_c[offset + i], sk_jazz[offset + i]);
    }
  }
  offset += PYR_IDX_BYTES;

  /* One seed for NFS implementations. */
  for (i = 0; i < PYR_N; ++i) {
    if (sk_c[offset + i] != sk_jazz[offset + i]) {
      res = -1;
      printf("[!] SK_SSD: sk_c[%zu] != sk_jazz[%zu] ==> %x != %x\n",
             i, i, sk_c[offset + i], sk_jazz[offset + i]);
    }
  }
  offset += PYR_N;

  for (i = 0; i < PYR_N; ++i) {
    if (sk_c[offset + i] != sk_jazz[offset + i]) {
      res = -1;
      printf("[!] SK_PRF: sk_c[%zu] != sk_jazz[%zu] ==> %x != %x\n", i, i,
             sk_c[offset + i], sk_jazz[offset + i]);
    }
  }
  offset += PYR_N;

  for (i = 0; i < PYR_N; ++i) {
    if (sk_c[offset + i] != sk_jazz[offset + i]) {
      res = -1;
      printf("[!] SK_PRT: sk_c[%zu] != sk_jazz[%zu] ==> %x != %x\n", i, i,
             sk_c[offset + i], sk_jazz[offset + i]);
    }
  }
  offset += PYR_N;

  for (i = 0; i < PYR_N; ++i) {
    if (sk_c[offset + i] != sk_jazz[offset + i]) {
      res = -1;
      printf("[!] SK_PSD: sk_c[%zu] != sk_jazz[%zu] ==> %x != %x\n", i, i,
             sk_c[offset + i], sk_jazz[offset + i]);
    }
  }
  offset += PYR_N;

  assert(offset == PYR_SK_CORE_BYTES);

  return res;
}

int cmp_PK(const unsigned char *pk_c, const unsigned char *pk_jazz) {
  int res = 0;
  size_t offset, i;
  offset = 0;

  for (i = 0; i < PYR_N; ++i) {
    if (pk_c[offset + i] != pk_jazz[offset + i]) {
      res = -1;
      printf("[!] PK_RT: pk_c[%zu] != pk_jazz[%zu] ==> %x != %x\n", i, i,
             pk_c[offset + i], pk_jazz[offset + i]);
    }
  }
  offset += PYR_N;

  for (i = 0; i < PYR_N; ++i) {
    if (pk_c[offset + i] != pk_jazz[offset + i]) {
      res = -1;
      printf("[!] PK_SD: pk_c[%zu] != pk_jazz[%zu] ==> %x != %x\n", i, i,
             pk_c[offset + i], pk_jazz[offset + i]);
    }
  }
  offset += PYR_N;

  assert(offset == PYR_PK_BYTES);

  return res;
}

int cmp_SK(const unsigned char *sk_c, const unsigned char *sk_jazz) {
  int res = 0;

  res |= cmp_SK_CORE(sk_c, sk_jazz);

  return res;
}

int cmp_SIG(const unsigned char *sig_c, const unsigned char *sig_jazz) {
  int res = 0;
  size_t offset, i, j;
  offset = 0;

  for (i = 0; i < PYR_IDX_BYTES; ++i) {
    if (sig_c[offset + i] != sig_jazz[offset + i]) {
      res = -1;
      printf("[!] SIG_IDX: sig_c[%zu] != sig_jazz[%zu] ==> %x != %x\n", i, i,
             sig_c[offset + i], sig_jazz[offset + i]);
    }
  }
  offset += PYR_IDX_BYTES;

  for (i = 0; i < PYR_N; ++i) {
    if (sig_c[offset + i] != sig_jazz[offset + i]) {
      res = -1;
      printf("[!] SIG_RND: sig_c[%zu] != sig_jazz[%zu] ==> %x != %x\n", i, i,
             sig_c[offset + i], sig_jazz[offset + i]);
    }
  }
  offset += PYR_N;

  for (i = 0; i < PYR_D; ++i) {
    for (j = 0; j < PYR_WOTS_BYTES; ++j) {
      if (sig_c[offset + j] != sig_jazz[offset + j]) {
        res = -1;
        printf(
            "[!] SIG_HTS[%zu][%zu] (XMSS): sig_c[%zu] != sig_jazz[%zu] ==> %x "
            "!= %x\n",
            i, j / PYR_N, j, j, sig_c[offset + j], sig_jazz[offset + j]);
      }
    }
    offset += PYR_WOTS_BYTES;
    for (j = 0; j < PYR_TREE_HEIGHT * PYR_N; ++j) {
      if (sig_c[offset + j] != sig_jazz[offset + j]) {
        res = -1;
        printf(
            "[!] SIG_HTS[%zu][%zu] (AUTH): sig_c[%zu] != sig_jazz[%zu] ==> %x "
            "!= %x\n",
            i, j / PYR_N, j, j, sig_c[offset + j], sig_jazz[offset + j]);
      }
    }
    offset += PYR_TREE_HEIGHT * PYR_N;
  }

  assert(offset == PYR_BYTES);

  return res;
}

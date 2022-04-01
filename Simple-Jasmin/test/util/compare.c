#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../../ref/api.h"
#include "../../ref/format.h"
#include "simple_BDS_state_format.h"
#include "simple_MMT_state_format.h"

int cmp_BDS_state(const unsigned char *bds_state_c,
                  const unsigned char *bds_state_jazz) {
  int res = 0;
  size_t offset, i;
  offset = 0;

  /* AUTH */
  for (i = 0; i < BDS_AUTH_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_AUTH[%zu]: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_N, i, i, bds_state_c[offset + i], bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_AUTH_SIZE;

  /* STCK */
  for (i = 0; i < BDS_ST_SZE_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_ST_SZE: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x != "
          "%x\n",
          i, i, bds_state_c[offset + i], bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_ST_SZE_SIZE;

  for (i = 0; i < BDS_ST_NDE_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_ST_NDE[%zu]: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_N, i, i, bds_state_c[offset + i], bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_ST_NDE_SIZE;

  for (i = 0; i < BDS_ST_HGT_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_ST_HGT[%zu]: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i, i, i, bds_state_c[offset + i], bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_ST_HGT_SIZE;

  /* TREE */
  for (i = 0; i < BDS_TH_NDE_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_TH_NDE[%zu]: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_N, i, i, bds_state_c[offset + i], bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_TH_NDE_SIZE;

  for (i = 0; i < BDS_TH_FIN_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_TH_FIN[%zu]: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i, i, i, bds_state_c[offset + i], bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_TH_FIN_SIZE;

  for (i = 0; i < BDS_TH_IDX_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_TH_IDX[%zu]: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_IDX_BYTES, i, i, bds_state_c[offset + i],
          bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_TH_IDX_SIZE;

  for (i = 0; i < BDS_TH_SDA_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_TH_SDA[%zu]: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_N, i, i, bds_state_c[offset + i], bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_TH_SDA_SIZE;

  for (i = 0; i < BDS_TH_SDN_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_TH_SDN[%zu]: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_N, i, i, bds_state_c[offset + i], bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_TH_SDN_SIZE;

  /* KEEP */
  for (i = 0; i < BDS_KEEP_SIZE; ++i) {
    if (bds_state_c[offset + i] != bds_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] BDS_KEEP[%zu]: bds_state_c[%zu] != bds_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_N, i, i, bds_state_c[offset + i], bds_state_jazz[offset + i]);
    }
  }
  offset += BDS_KEEP_SIZE;

  assert(offset == BDS_SIZE);

  return res;
}

int cmp_MMT_state(const unsigned char *mmt_state_c,
                  const unsigned char *mmt_state_jazz) {
  int res = 0;
  size_t offset, i;
  offset = 0;

  /* AUTH */
  for (i = 0; i < MMT_AUTH_SIZE; ++i) {
    if (mmt_state_c[offset + i] != mmt_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] MMT_AUTH[%zu]: mmt_state_c[%zu] != mmt_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_N, i, i, mmt_state_c[offset + i], mmt_state_jazz[offset + i]);
    }
  }
  offset += MMT_AUTH_SIZE;

  /* STCK */
  for (i = 0; i < MMT_ST_SZE_SIZE; ++i) {
    if (mmt_state_c[offset + i] != mmt_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] MMT_ST_SZE: mmt_state_c[%zu] != mmt_state_jazz[%zu] ==> %x != "
          "%x\n",
          i, i, mmt_state_c[offset + i], mmt_state_jazz[offset + i]);
    }
  }
  offset += MMT_ST_SZE_SIZE;

  for (i = 0; i < MMT_ST_NDE_SIZE; ++i) {
    if (mmt_state_c[offset + i] != mmt_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] MMT_ST_NDE[%zu]: mmt_state_c[%zu] != mmt_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_N, i, i, mmt_state_c[offset + i], mmt_state_jazz[offset + i]);
    }
  }
  offset += MMT_ST_NDE_SIZE;

  for (i = 0; i < MMT_ST_HGT_SIZE; ++i) {
    if (mmt_state_c[offset + i] != mmt_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] MMT_ST_HGT[%zu]: mmt_state_c[%zu] != mmt_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i, i, i, mmt_state_c[offset + i], mmt_state_jazz[offset + i]);
    }
  }
  offset += MMT_ST_HGT_SIZE;

  /* TREE */
  for (i = 0; i < MMT_TH_FIN_SIZE; ++i) {
    if (mmt_state_c[offset + i] != mmt_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] MMT_TH_FIN[%zu]: mmt_state_c[%zu] != mmt_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i, i, i, mmt_state_c[offset + i], mmt_state_jazz[offset + i]);
    }
  }
  offset += MMT_TH_FIN_SIZE;

  for (i = 0; i < MMT_TH_IDX_SIZE; ++i) {
    if (mmt_state_c[offset + i] != mmt_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] MMT_TH_IDX[%zu]: mmt_state_c[%zu] != mmt_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_IDX_BYTES, i, i, mmt_state_c[offset + i],
          mmt_state_jazz[offset + i]);
    }
  }
  offset += MMT_TH_IDX_SIZE;

  for (i = 0; i < MMT_TH_SDA_SIZE; ++i) {
    if (mmt_state_c[offset + i] != mmt_state_jazz[offset + i]) {
      res = -1;
      printf(
          "[!] MMT_TH_SDA[%zu]: mmt_state_c[%zu] != mmt_state_jazz[%zu] ==> %x "
          "!= %x\n",
          i / PYR_N, i, i, mmt_state_c[offset + i], mmt_state_jazz[offset + i]);
    }
  }
  offset += MMT_TH_SDA_SIZE;

  assert(offset == MMT_SIZE);

  return res;
}

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

  for (i = 0; i < PYR_D * PYR_N; ++i) {
    if (sk_c[offset + i] != sk_jazz[offset + i]) {
      res = -1;
      printf("[!] SK_SSD[%zu]: sk_c[%zu] != sk_jazz[%zu] ==> %x != %x\n",
             i / PYR_N, i, i, sk_c[offset + i], sk_jazz[offset + i]);
    }
  }
  offset += PYR_D * PYR_N;

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
  size_t i, offset;
  offset = 0;

  res |= cmp_SK_CORE(sk_c + offset, sk_jazz + offset);
  offset += PYR_SK_CORE_BYTES;

  res |= cmp_BDS_state(sk_c + offset, sk_jazz + offset);
  offset += BDS_BYTES;

  for (i = 1; i < PYR_D; ++i) {
    res |= cmp_MMT_state(sk_c + offset, sk_jazz + offset);
    offset += MMT_BYTES;
  }

  assert(offset == CRYPTO_SECRETKEYBYTES);

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

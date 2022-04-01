#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/params.h"
#include "../ref/wots.h"
#include "util/randomtestdata.h"

#define CSUM_BYTES ((PYR_WOTS_LEN2 * PYR_WOTS_LOGW + 7) / 8)
#define START ((PYR_WOTS_W - 1) / 4)
#define STEPS (((PYR_WOTS_W - 1) - START) / 2)

#if STEPS == 0
#warning gen_chain unit test does not advance in chain.
#endif

int test_expand_seed() {
  int res = 0;
  size_t i;
  unsigned char output_c[PYR_WOTS_PK_BYTES];
  unsigned char output_jazz[PYR_WOTS_PK_BYTES];

  unsigned char inseed[PYR_N];
  unsigned char pub_seed[PYR_N];
  uint32_t addr[8];

  random_test_bytes(inseed, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(addr, 8);

  expand_seed(output_c, inseed, pub_seed, addr);
  expand_seed_jazz(output_jazz, inseed, pub_seed, addr);

  for (i = 0; i < PYR_WOTS_PK_BYTES; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\texpand_seed.\nReason:\toutput_c[%zu] != "
          "output_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }

  return res;
}

int test_gen_chain() {
  int res = 0;
  size_t i;
  unsigned char output_c[PYR_N];
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  unsigned char input_c[PYR_N];
  unsigned char inout_jazz[PYR_N];
  unsigned char pub_seed[PYR_N];

  random_test_bytes(input_c, PYR_N);
  memcpy(inout_jazz, input_c, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(addr_c, 8);
  memcpy(addr_jazz, addr_c, 32);

  gen_chain(output_c, input_c, START, STEPS, pub_seed, addr_c);
  gen_chain_jazz(inout_jazz, START, STEPS, pub_seed, addr_jazz);

  for (i = 0; i < PYR_N; ++i) {
    if (output_c[i] != inout_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tgen_chain.\nReason:\toutput_c[%zu] != "
          "inout_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], inout_jazz[i]);
    }
  }
  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tgen_chain.\nReason:\taddrc[%zu] "
          "!= addrc[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_base_w_m_jazz() {
  int res = 0;
  size_t i;
  uint32_t output_c[PYR_WOTS_LEN1];
  uint32_t output_jazz[PYR_WOTS_LEN1];

  unsigned char input[PYR_N];

  random_test_bytes(input, PYR_N);

  base_w(output_c, PYR_WOTS_LEN1, input);
  base_w_m_jazz(output_jazz, input);

  for (i = 0; i < PYR_WOTS_LEN1; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tbase_w_m.\nReason:\toutput_c[%zu] "
          "!= output_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }

  return res;
}

int test_base_w_c_jazz() {
  int res = 0;
  size_t i;
  uint32_t output_c[PYR_WOTS_LEN2];
  uint32_t output_jazz[PYR_WOTS_LEN2];

  unsigned char input[CSUM_BYTES];

  random_test_bytes(input, CSUM_BYTES);

  base_w(output_c, PYR_WOTS_LEN2, input);
  base_w_c_jazz(output_jazz, input);

  for (i = 0; i < PYR_WOTS_LEN2; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tbase_w_c.\nReason:\toutput_c[%zu] "
          "!= output_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }

  return res;
}

int test_wots_checksum_jazz() {
  int res = 0;
  size_t i;
  uint32_t output_c[PYR_WOTS_LEN2];
  uint32_t output_jazz[PYR_WOTS_LEN2];

  uint32_t input[PYR_WOTS_LEN1];

  random_test_32bit_blocks(input, PYR_WOTS_LEN1);
  for (i = 0; i < PYR_WOTS_LEN1; ++i) {
    input[i] &= PYR_WOTS_W - 1;
  }

  wots_checksum(output_c, input);
  wots_checksum_jazz(output_jazz, input);

  for (i = 0; i < PYR_WOTS_LEN2; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit input "
          "failed.\nFunction:\twots_checksum.\nReason:\toutput_c[%zu] != "
          "output_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }

  return res;
}

int test_chain_lengths_jazz() {
  int res = 0;
  size_t i;
  uint32_t output_c[PYR_WOTS_LEN];
  uint32_t output_jazz[PYR_WOTS_LEN];

  unsigned char input[PYR_N];

  random_test_bytes(input, PYR_N);

  chain_lengths(output_c, input);
  chain_lengths(output_jazz, input);

  for (i = 0; i < PYR_WOTS_LEN2; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tchain_lengths.\nReason:\toutput_c[%zu] != "
          "output_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }

  return res;
}

int test_wots_pkgen_jazz() {
  int res = 0;
  size_t i;
  unsigned char output_c[PYR_WOTS_PK_BYTES];
  unsigned char output_jazz[PYR_WOTS_PK_BYTES];
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  unsigned char seed[PYR_N];
  unsigned char pub_seed[PYR_N];

  random_test_bytes(seed, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(addr_c, 8);
  memcpy(addr_jazz, addr_c, 32);

  wots_pkgen(output_c, seed, pub_seed, addr_c);
  wots_pkgen_jazz(output_jazz, seed, pub_seed, addr_jazz);

  for (i = 0; i < PYR_WOTS_PK_BYTES; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\twots_pkgen.\nReason:\toutput_c[%zu] != "
          "output_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }
  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\twots_sign.\nReason:\taddr_c[%zu] "
          "!= addr_jazz[%zu] ==> %d != %d\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_wots_sign_jazz() {
  int res = 0;
  size_t i;
  unsigned char output_c[PYR_WOTS_BYTES];
  unsigned char output_jazz[PYR_WOTS_BYTES];
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  unsigned char input[PYR_N];
  unsigned char seed[PYR_N];
  unsigned char pub_seed[PYR_N];

  random_test_bytes(input, PYR_N);
  random_test_bytes(seed, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(addr_c, 8);
  memcpy(addr_jazz, addr_c, 32);

  wots_sign(output_c, input, seed, pub_seed, addr_c);
  wots_sign_jazz(output_jazz, input, seed, pub_seed, addr_jazz);

  for (i = 0; i < PYR_WOTS_BYTES; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\twots_sign.\nReason:\toutput_c[%zu] != "
          "output_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }
  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\twots_sign.\nReason:\taddr_c[%zu] "
          "!= addr_jazz[%zu] ==> %d != %d\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_pk_from_sig_jazz() {
  int res = 0;
  size_t i;
  unsigned char output_c[PYR_WOTS_PK_BYTES];
  unsigned char output_jazz[PYR_WOTS_PK_BYTES];
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  unsigned char sig[PYR_WOTS_BYTES];
  unsigned char input[PYR_N];
  unsigned char seed[PYR_N];
  unsigned char pub_seed[PYR_N];

  random_test_bytes(sig, PYR_WOTS_BYTES);
  random_test_bytes(input, PYR_N);
  random_test_bytes(seed, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(addr_c, 8);
  memcpy(addr_jazz, addr_c, 32);

  wots_pk_from_sig(output_c, sig, input, pub_seed, addr_c);
  wots_pk_from_sig_jazz(output_jazz, sig, input, pub_seed, addr_jazz);

  for (i = 0; i < PYR_WOTS_PK_BYTES; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\twots_pk_from_sig.\nReason:\toutput_c[%zu] != "
          "output_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }
  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\twots_pk_from_sig.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %d != %d\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_gen_leaf_wots_jazz() {
  int res = 0;
  size_t i;
  unsigned char output_c[PYR_N];
  unsigned char output_jazz[PYR_N];
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  unsigned char seed[PYR_N];
  unsigned char pub_seed[PYR_N];
  uint32_t addr[8];

  random_test_bytes(seed, PYR_N);
  random_test_bytes(pub_seed, PYR_N);
  random_test_32bit_blocks(addr_c, 8);
  memcpy(addr_jazz, addr_c, 32);
  random_test_32bit_blocks(addr, 8);

  gen_leaf_wots(output_c, seed, pub_seed, addr, addr_c);
  gen_leaf_wots_jazz(output_jazz, seed, pub_seed, addr, addr_jazz);

  for (i = 0; i < PYR_N; ++i) {
    if (output_c[i] != output_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tgen_leaf_wots.\nReason:\toutput_c[%zu] != "
          "output_jazz[%zu] ==> %d != %d\n",
          i, i, output_c[i], output_jazz[i]);
    }
  }
  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tgen_leaf_wots.\nReason:\taddr1_c[%zu] != "
          "addr1_jazz[%zu] ==> %d != %d\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int main() {
  int res = 0;

  res |= test_expand_seed();
  res |= test_gen_chain();
  res |= test_base_w_m_jazz();
  res |= test_base_w_c_jazz();
  res |= test_wots_checksum_jazz();
  res |= test_chain_lengths_jazz();
  res |= test_wots_pkgen_jazz();
  res |= test_wots_sign_jazz();
  res |= test_pk_from_sig_jazz();
  res |= test_gen_leaf_wots_jazz();

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

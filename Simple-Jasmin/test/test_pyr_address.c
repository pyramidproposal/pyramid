#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/address.h"
#include "../ref/params.h"
#include "util/randomtestdata.h"

#define INLEN (2 * SHAKE256_RATE + 10)

int test_set_layer_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  set_layer_addr(addr_c, 0xff);
  set_layer_addr_jazz(addr_jazz, 0xff);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tset_layer_addr.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %u != %u\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_set_tree_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  uint64_t tree_addr;

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  random_test_64bit_blocks(&tree_addr, 1);

  set_tree_addr(addr_c, tree_addr);
  set_tree_addr_jazz(addr_jazz, tree_addr);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tset_tree_addr.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_set_type_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  set_type(addr_c, 0xff);
  set_type_0xff_jazz(addr_jazz); /* Type is compile-time. */

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tset_type.\nReason:\taddr_c[%zu] "
          "!= addr_jazz[%zu] ==> %u != %u\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_copy_subtree_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  uint32_t src_addr[8];

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  random_test_32bit_blocks(src_addr, 8);

  copy_subtree_addr(addr_c, src_addr);
  copy_subtree_addr_jazz(addr_jazz, src_addr);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tcopy_subtree_addr.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %u != %u\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_set_keypair_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  uint32_t keypair_addr;

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  random_test_32bit_blocks(&keypair_addr, 1);

  set_keypair_addr(addr_c, keypair_addr);
  set_keypair_addr_jazz(addr_jazz, keypair_addr);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tset_keypair_addr.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_copy_keypair_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8] = {0};
  uint32_t addr_jazz[8] = {0};

  copy_keypair_addr(addr_c, addr);
  copy_keypair_addr_jazz(addr_jazz, addr);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tcopy_keypair_addr.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %u != %u\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_set_chain_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  unsigned char chain;

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  random_test_bytes(&chain, 1);

  set_chain_addr(addr_c, chain);
  set_chain_addr_jazz(addr_jazz, chain);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tset_chain_addr.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_set_hash_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  unsigned char hash;

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  random_test_bytes(&hash, 1);

  set_hash_addr(addr_c, hash);
  set_hash_addr_jazz(addr_jazz, hash);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tset_hash_addr.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_set_tree_height_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  unsigned char tree_height;

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  random_test_bytes(&tree_height, 1);

  set_tree_height(addr_c, 0xff);
  set_tree_height_jazz(addr_jazz, 0xff);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tset_tree_height.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_set_tree_index_addr(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  uint64_t tree_index;

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  random_test_64bit_blocks(&tree_index, 1);

  set_tree_index(addr_c, tree_index);
  set_tree_index_jazz(addr_jazz, tree_index);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tset_tree_index.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_get_tree_addr(const uint32_t addr[8]) {
  int res = 0;

  uint64_t out_c, out_jazz;

  out_c = get_tree_addr(addr);
  out_jazz = get_tree_addr_jazz(addr);

  if (out_c != out_jazz) {
    res = -1;
    printf(
        "[!] Unit test failed.\nFunction:\tget_tree_addr.\nReason:\tout_c != "
        "out_jazz ==> %lx != %lx\n",
        out_c, out_jazz);
  }

  return res;
}

int test_get_keypair_addr(const uint32_t addr[8]) {
  int res = 0;

  uint64_t out_c, out_jazz;

  out_c = get_keypair_addr(addr);
  out_jazz = get_keypair_addr_jazz(addr);

  if (out_c != out_jazz) {
    res = -1;
    printf(
        "[!] Unit test failed.\nFunction:\tget_keypair_addr.\nReason:\tout_c "
        "!= out_c ==> %lx != %lx\n",
        out_c, out_jazz);
  }

  return res;
}

int test_set_keypair_addr_bottom(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  uint64_t index;

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  random_test_64bit_blocks(&index, 1);

  set_tree_index(addr_c, index);
  set_tree_index_jazz(addr_jazz, index);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tset_keypair_addr_bottom.\nReason:\taddr_c[%zu] "
          "!= addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_increment_address(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  increment_address(addr_c);
  increment_address_jazz(addr_jazz);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tincrement_address.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_increment_address_overflow(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  set_keypair_addr(addr_c, (1ULL << PYR_TREE_HEIGHT) - 1);
  set_keypair_addr(addr_jazz, (1ULL << PYR_TREE_HEIGHT) - 1);

  increment_address(addr_c);
  increment_address_jazz(addr_jazz);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tincrement_address "
          "(overflow).\nReason:\taddr_c[%zu] != addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int test_set_direction(const uint32_t addr[8]) {
  int res = 0;
  size_t i;
  uint32_t addr_c[8];
  uint32_t addr_jazz[8];

  unsigned char direction;

  memcpy(addr_c, addr, 32);
  memcpy(addr_jazz, addr, 32);

  random_test_bytes(&direction, 1);

  set_direction(addr_c, direction);
  set_direction_jazz(addr_jazz, direction);

  for (i = 0; i < 8; ++i) {
    if (addr_c[i] != addr_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tset_direction.\nReason:\taddr_c[%zu] != "
          "addr_jazz[%zu] ==> %x != %x\n",
          i, i, addr_c[i], addr_jazz[i]);
    }
  }

  return res;
}

int main() {
  int res = 0;
  uint32_t addr[8];
  random_test_32bit_blocks(addr, 8);

  res |= test_set_layer_addr(addr);
  res |= test_set_tree_addr(addr);
  res |= test_set_tree_addr(addr);
  res |= test_set_type_addr(addr);
  res |= test_copy_subtree_addr(addr);
  res |= test_set_keypair_addr(addr);
  res |= test_copy_keypair_addr(addr);
  res |= test_set_chain_addr(addr);
  res |= test_set_hash_addr(addr);
  res |= test_set_tree_height_addr(addr);
  res |= test_set_tree_index_addr(addr);
  res |= test_get_tree_addr(addr);
  res |= test_get_keypair_addr(addr);
  res |= test_set_keypair_addr_bottom(addr);
  res |= test_increment_address(addr);
  res |= test_increment_address_overflow(addr);
  res |= test_set_direction(addr);

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

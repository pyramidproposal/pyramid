#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/params.h"
#include "../ref/simple_BDS_state.h"
#include "../ref/xmss_simple.h"
#include "util/randomtestdata.h"
#include "util/simple_BDS_state_format.h"

/* BDS state */
#define STATE_TYPE 0

/* TH_INDEX chooses the treehash instance */
#define TH_INDEX (PYR_TREE_HEIGHT / 2)
/* ST_INDEX chooses the stack item */
#define ST_INDEX ((PYR_TREE_HEIGHT - 1) / 2)
/* KE_INDEX chooses the keep node */
#define KE_INDEX ((PYR_TREE_HEIGHT / 2) / 2)
/* AU_INDEX chooses the auth node */
#define AU_INDEX (PYR_TREE_HEIGHT / 2)

#if ST_INDEX == 0
#error Pop test undefined.
#endif

int test_BDS_th_get_nde(const unsigned char *sk) {
  int res = 0;
  size_t i;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char node_c[PYR_N];
  unsigned char node_jazz[PYR_N];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  memcpy(node_c, (state_c.treehash + TH_INDEX)->node, PYR_N);
  BDS_th_get_nde_jazz(node_jazz, sk, TH_INDEX);

  for (i = 0; i < PYR_N; ++i) {
    if (node_c[i] != node_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_th_get_nde.\nReason:\tnode_c[%zu] != "
          "node_jazz[%zu] ==> %x != %x\n",
          i, i, node_c[i], node_jazz[i]);
    }
  }

  return res;
}

int test_BDS_th_get_fin(const unsigned char *sk) {
  int res = 0;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char fin_c, fin_jazz;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  fin_c = (state_c.treehash + TH_INDEX)->fin;
  fin_jazz = BDS_th_get_fin_jazz(sk, TH_INDEX);

  if (fin_c != fin_jazz) {
    res = -1;
    printf(
        "[!] Unit test failed.\nFunction:\tBDS_th_get_fin.\nReason:\tfin_c != "
        "fin_jazz ==> %x != %x\n",
        fin_c, fin_jazz);
  }

  return res;
}

int test_BDS_th_get_idx(const unsigned char *sk) {
  int res = 0;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  uint64_t idx_c, idx_jazz;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  idx_c = (state_c.treehash + TH_INDEX)->idx;
  idx_jazz = BDS_th_get_idx_jazz(sk, TH_INDEX);

  if (idx_c != idx_jazz) {
    res = -1;
    printf(
        "[!] Unit test failed.\nFunction:\tBDS_th_get_idx.\nReason:\tidx_c != "
        "idx_jazz ==> %lx != %lx\n",
        idx_c, idx_jazz);
  }

  return res;
}

int test_BDS_th_get_sda(const unsigned char *sk) {
  int res = 0;
  size_t i;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char sda_c[PYR_N];
  unsigned char sda_jazz[PYR_N];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  memcpy(sda_c, (state_c.treehash + TH_INDEX)->seed_active, PYR_N);
  BDS_th_get_sda_jazz(sda_jazz, sk, TH_INDEX);

  for (i = 0; i < PYR_N; ++i) {
    if (sda_c[i] != sda_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_th_get_sda.\nReason:\tsda_c[%zu] != "
          "sda_jazz[%zu] ==> %x != %x\n",
          i, i, sda_c[i], sda_jazz[i]);
    }
  }

  return res;
}

int test_BDS_th_get_sdn(const unsigned char *sk) {
  int res = 0;
  size_t i;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char sdn_c[PYR_N];
  unsigned char sdn_jazz[PYR_N];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  memcpy(sdn_c, (state_c.treehash + TH_INDEX)->seed_next, PYR_N);
  BDS_th_get_sdn_jazz(sdn_jazz, sk, TH_INDEX);

  for (i = 0; i < PYR_N; ++i) {
    if (sdn_c[i] != sdn_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_th_get_sdn.\nReason:\tsdn_c[%zu] != "
          "sdn_jazz[%zu] ==> %x != %x\n",
          i, i, sdn_c[i], sdn_jazz[i]);
    }
  }

  return res;
}

int test_BDS_st_get_size(const unsigned char *sk) {
  int res = 0;
  const unsigned char *stack = sk + BDS_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char size_c, size_jazz;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  size_c = state_c.stack->size;
  size_jazz = BDS_st_get_size_jazz(stack);

  if (size_c != size_jazz) {
    res = -1;
    printf(
        "[!] Unit test failed.\nFunction:\tBDS_st_get_size.\nReason:\tsize_c "
        "!= size_jazz ==> %x != %x\n",
        size_c, size_jazz);
  }

  return res;
}

int test_BDS_st_get_height(const unsigned char *sk) {
  int res = 0;
  const unsigned char *stack = sk + BDS_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char height_c, height_jazz;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  height_c = state_c.stack->nodeheight[ST_INDEX];
  height_jazz = BDS_st_get_height_jazz(stack, ST_INDEX);

  if (height_c != height_jazz) {
    res = -1;
    printf(
        "[!] Unit test "
        "failed.\nFunction:\tBDS_st_get_height.\nReason:\theight_c != "
        "height_jazz ==> %x != %x\n",
        height_c, height_jazz);
  }

  return res;
}

int test_BDS_st_set_size(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  unsigned char *stack = sk_jazz + BDS_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  unsigned char size;

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  random_test_bytes(&size, 1);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  state_c.stack->size = size;
  BDS_st_set_size_jazz(stack, size);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_st_set_size.\nReason:\tsk_c[%zu] != "
          "sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_st_set_height(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  unsigned char *stack = sk_jazz + BDS_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  unsigned char height;

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  random_test_bytes(&height, 1);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  (state_c.stack)->nodeheight[ST_INDEX] = height;
  BDS_st_set_height_jazz(stack, height, ST_INDEX);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_st_set_height.\nReason:\tsk_c[%zu] != "
          "sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_stack_pop(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  unsigned char *stack_jazz = sk_jazz + BDS_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char pop_node_c[PYR_N];
  unsigned char pop_node_jazz[PYR_N];

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  stack_pop(stack_c, pop_node_c);
  BDS_stack_pop_jazz(pop_node_jazz, stack_jazz);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < PYR_N; ++i) {
    if (pop_node_c[i] != pop_node_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_stack_pop.\nReason:\tpop_node_c[%zu] != "
          "pop_node_jazz[%zu] ==> %x != %x\n",
          i, i, pop_node_c[i], pop_node_jazz[i]);
    }
  }

  for (i = 0; i < BDS_ST_SIZE; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_stack_pop.\nReason:\tstack__c[%zu] != "
          "stack__jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_stack_push(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  unsigned char *stack_jazz = sk_jazz + BDS_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  unsigned char push_node_height;

  unsigned char push_node_c[PYR_N];
  unsigned char push_node_jazz[PYR_N];

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  random_test_bytes(&push_node_height, 1);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  stack_push(state_c.stack, push_node_c, push_node_height);
  BDS_stack_push_jazz(stack_jazz, push_node_jazz, push_node_height);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < BDS_ST_SIZE; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_stack_push.\nReason:\tstack__c[%zu] != "
          "stack__jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  return res;
}

int test_BDS_get_keep(const unsigned char *sk) {
  int res = 0;
  size_t i;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char keep_c[PYR_N];
  unsigned char keep_jazz[PYR_N];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  memcpy(keep_c, state_c.keep + (KE_INDEX * PYR_N), PYR_N);
  BDS_get_keep_jazz(keep_jazz, sk, KE_INDEX);

  for (i = 0; i < PYR_N; ++i) {
    if (keep_c[i] != keep_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_get_keep.\nReason:\tkeep_c[%zu] != "
          "keep_jazz[%zu] ==> %x != %x\n",
          i, i, keep_c[i], keep_jazz[i]);
    }
  }

  return res;
}

int test_BDS_th_set_nde(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char node[PYR_N];

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  random_test_bytes(node, PYR_N);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  memcpy((state_c.treehash + TH_INDEX)->node, node, PYR_N);
  BDS_th_set_nde_jazz(sk_jazz, node, TH_INDEX);

  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_th_set_nde.\nReason:\tsk_c[%zu] != "
          "sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_th_set_fin_0(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  (state_c.treehash + TH_INDEX)->fin = 0;
  BDS_th_set_fin_0_jazz(sk_jazz, TH_INDEX);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_th_set_fin_0.\nReason:\tsk_c[%zu] != "
          "sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_th_set_fin_1(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  (state_c.treehash + TH_INDEX)->fin = 1;
  BDS_th_set_fin_1_jazz(sk_jazz, TH_INDEX);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_th_set_fin_1.\nReason:\tsk_c[%zu] != "
          "sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_th_set_idx(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  uint64_t idx;

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  random_test_64bit_blocks(&idx, 1);
  idx &= (1 << PYR_FULL_HEIGHT) - 1;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  (state_c.treehash + TH_INDEX)->idx = idx;
  BDS_th_set_idx_jazz(sk_jazz, idx, TH_INDEX);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_th_set_idx.\nReason:\tsk_c[%zu] != "
          "sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_th_set_sda(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char node[PYR_N];

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  random_test_bytes(node, PYR_N);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  memcpy((state_c.treehash + TH_INDEX)->seed_active, node, PYR_N);
  BDS_th_set_sda_jazz(sk_jazz, node, TH_INDEX);

  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_th_set_sda.\nReason:\tsk_c[%zu] != "
          "sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_th_set_sdn(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char node[PYR_N];

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  random_test_bytes(node, PYR_N);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  memcpy((state_c.treehash + TH_INDEX)->seed_next, node, PYR_N);
  BDS_th_set_sdn_jazz(sk_jazz, node, TH_INDEX);

  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_th_set_sdn.\nReason:\tsk_c[%zu] != "
          "sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_get_auth_node(const unsigned char *sk) {
  int res = 0;
  size_t i;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char node_c[PYR_N];
  unsigned char node_jazz[PYR_N];

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  memcpy(node_c, state_c.auth + (AU_INDEX * PYR_N), PYR_N);
  BDS_get_auth_node_jazz(node_jazz, sk, TH_INDEX);

  for (i = 0; i < PYR_N; ++i) {
    if (node_c[i] != node_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_get_auth_node.\nReason:\tnode_c[%zu] != "
          "node_jazz[%zu] ==> %x != %x\n",
          i, i, node_c[i], node_jazz[i]);
    }
  }

  return res;
}

int test_BDS_set_auth_node(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char node[PYR_N];

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  random_test_bytes(node, PYR_N);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  memcpy(state_c.auth + (AU_INDEX * PYR_N), node, PYR_N);
  BDS_set_auth_node_jazz(sk_jazz, node, AU_INDEX);

  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test "
          "failed.\nFunction:\tBDS_set_auth_node.\nReason:\tsk_c[%zu] != "
          "sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_BDS_set_keep(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[BDS_BYTES];
  unsigned char sk_jazz[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char node[PYR_N];

  memcpy(sk_c, sk, BDS_BYTES);
  memcpy(sk_jazz, sk, BDS_BYTES);

  random_test_bytes(node, PYR_N);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  memcpy(state_c.keep + (KE_INDEX * PYR_N), node, PYR_N);
  BDS_set_keep_jazz(sk_jazz, node, KE_INDEX);

  for (i = 0; i < BDS_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
          "[!] Unit test failed.\nFunction:\tBDS_set_keep.\nReason:\tsk_c[%zu] "
          "!= sk_jazz[%zu] ==> %x != %x\n",
          i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int main() {
  int res = 0;
  unsigned char sk_c[BDS_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  unsigned char push_node[PYR_N];
  unsigned char push_node_height;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  random_test_bytes(sk_c, BDS_BYTES);
  random_test_bytes(&push_node_height, 1);
  random_test_bytes(push_node, PYR_N);

  /* Ensure non-empty stack for pop test. */
  state_deserialize(sk_c, &state_c, STATE_TYPE);
  state_c.stack->size = 0;
  stack_push(state_c.stack, push_node, push_node_height);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);

  res |= test_BDS_th_get_nde(sk_c);
  res |= test_BDS_th_get_fin(sk_c);
  res |= test_BDS_th_get_idx(sk_c);
  res |= test_BDS_th_get_sda(sk_c);
  res |= test_BDS_th_get_sdn(sk_c);

  res |= test_BDS_th_set_nde(sk_c);
  res |= test_BDS_th_set_fin_0(sk_c);
  res |= test_BDS_th_set_fin_1(sk_c);
  res |= test_BDS_th_set_idx(sk_c);
  res |= test_BDS_th_set_sda(sk_c);
  res |= test_BDS_th_set_sdn(sk_c);

  res |= test_BDS_st_get_size(sk_c);
  res |= test_BDS_st_get_height(sk_c);
  res |= test_BDS_st_set_size(sk_c);
  res |= test_BDS_st_set_height(sk_c);

  res |= test_BDS_stack_pop(sk_c);
  res |= test_BDS_stack_push(sk_c);

  res |= test_BDS_get_auth_node(sk_c);
  res |= test_BDS_set_auth_node(sk_c);

  res |= test_BDS_get_keep(sk_c);
  res |= test_BDS_set_keep(sk_c);

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

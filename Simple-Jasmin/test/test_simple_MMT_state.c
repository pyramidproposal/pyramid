#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../ref/params.h"
#include "../ref/simple_MMT_state.h"
#include "../ref/pyr_simple.h"
#include "util/randomtestdata.h"
#include "util/simple_MMT_state_format.h"

/* MMT state */
#define STATE_TYPE 1

/* TH_INDEX chooses the treehash instance */
#define TH_INDEX (PYR_TREE_HEIGHT / 2)
/* ST_INDEX chooses the stack item */
#define ST_INDEX (PYR_TREE_HEIGHT / 2)
/* AU_INDEX chooses the auth node */
#define AU_INDEX (PYR_TREE_HEIGHT / 2)

int test_MMT_th_get_fin(const unsigned char *sk) {
  int res = 0;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char fin_c, fin_jazz;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  fin_c = (state_c.treehash + TH_INDEX)->fin;
  fin_jazz = MMT_th_get_fin_jazz(sk, TH_INDEX);

  if (fin_c != fin_jazz) {
    res = -1;
    printf(
	   "[!] Unit test failed.\nFunction:\tMMT_th_get_fin.\nReason:\tfin_c != "
	   "fin_jazz ==> %x != %x\n",
	   fin_c, fin_jazz);
  }

  return res;
}

int test_MMT_th_get_idx(const unsigned char *sk) {
  int res = 0;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  uint64_t idx_c, idx_jazz;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  idx_c = (state_c.treehash + TH_INDEX)->idx;
  idx_jazz = MMT_th_get_idx_jazz(sk, TH_INDEX);

  if (idx_c != idx_jazz) {
    res = -1;
    printf(
	   "[!] Unit test failed.\nFunction:\tMMT_th_get_idx.\nReason:\tidx_c != "
	   "idx_jazz ==> %lx != %lx\n",
	   idx_c, idx_jazz);
  }

  return res;
}

int test_MMT_th_get_sda(const unsigned char *sk) {
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

  memcpy(sda_c, (state_c.treehash + TH_INDEX)->ref_active, PYR_N);
  MMT_th_get_sda_jazz(sda_jazz, sk, TH_INDEX);

  for (i = 0; i < PYR_N; ++i) {
    if (sda_c[i] != sda_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_th_get_sda.\nReason:\tsda_c[%zu] != "
	     "sda_jazz[%zu] ==> %x != %x\n",
	     i, i, sda_c[i], sda_jazz[i]);
    }
  }

  return res;
}

int test_MMT_st_get_size(const unsigned char *sk) {
  int res = 0;
  const unsigned char *stack = sk + MMT_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char size_c, size_jazz;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  size_c = state_c.stack->size;
  size_jazz = MMT_st_get_size_jazz(stack);

  if (size_c != size_jazz) {
    res = -1;
    printf(
	   "[!] Unit test failed.\nFunction:\tMMT_st_get_size.\nReason:\tsize_c "
	   "!= size_jazz ==> %x != %x\n",
	   size_c, size_jazz);
  }

  return res;
}

int test_MMT_st_get_height(const unsigned char *sk) {
  int res = 0;
  const unsigned char *stack = sk + MMT_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char height_c, height_jazz;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk, &state_c, STATE_TYPE);

  height_c = state_c.stack->nodeheight[ST_INDEX];
  height_jazz = MMT_st_get_height_jazz(stack, ST_INDEX);

  if (height_c != height_jazz) {
    res = -1;
    printf(
	   "[!] Unit test "
	   "failed.\nFunction:\tMMT_st_get_height.\nReason:\theight_c != "
	   "height_jazz ==> %x != %x\n",
	   height_c, height_jazz);
  }

  return res;
}

int test_MMT_st_set_size(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  unsigned char *stack = sk_jazz + MMT_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  unsigned char size;

  memcpy(sk_c, sk, MMT_BYTES);
  memcpy(sk_jazz, sk, MMT_BYTES);

  random_test_bytes(&size, 1);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  state_c.stack->size = size;
  MMT_st_set_size_jazz(stack, size);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < MMT_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_st_set_size.\nReason:\tsk_c[%zu] != "
	     "sk_jazz[%zu] ==> %x != %x\n",
	     i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_MMT_st_set_height(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  unsigned char *stack = sk_jazz + MMT_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  unsigned char height;

  memcpy(sk_c, sk, MMT_BYTES);
  memcpy(sk_jazz, sk, MMT_BYTES);

  random_test_bytes(&height, 1);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  (state_c.stack)->nodeheight[ST_INDEX] = height;
  MMT_st_set_height_jazz(stack, height, ST_INDEX);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < MMT_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_st_set_height.\nReason:\tsk_c[%zu] != "
	     "sk_jazz[%zu] ==> %x != %x\n",
	     i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_MMT_stack_pop(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  unsigned char *stack_jazz = sk_jazz + MMT_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char pop_node_c[PYR_N];
  unsigned char pop_node_jazz[PYR_N];

  memcpy(sk_c, sk, MMT_BYTES);
  memcpy(sk_jazz, sk, MMT_BYTES);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  stack_pop(stack_c, pop_node_c);
  MMT_stack_pop_jazz(pop_node_jazz, stack_jazz);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < PYR_N; ++i) {
    if (pop_node_c[i] != pop_node_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_stack_pop.\nReason:\tpop_node_c[%zu] != "
	     "pop_node_jazz[%zu] ==> %x != %x\n",
	     i, i, pop_node_c[i], pop_node_jazz[i]);
    }
  }

  for (i = 0; i < MMT_ST_SIZE; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_stack_pop.\nReason:\tstack__c[%zu] != "
	     "stack__jazz[%zu] ==> %x != %x\n",
	     i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_MMT_stack_push(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  unsigned char *stack_jazz = sk_jazz + MMT_STCK;
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  unsigned char push_node_height;

  unsigned char push_node_c[PYR_N];
  unsigned char push_node_jazz[PYR_N];

  memcpy(sk_c, sk, MMT_BYTES);
  memcpy(sk_jazz, sk, MMT_BYTES);

  random_test_bytes(&push_node_height, 1);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  stack_push(state_c.stack, push_node_c, push_node_height);
  MMT_stack_push_jazz(stack_jazz, push_node_jazz, push_node_height);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < MMT_ST_SIZE; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_stack_push.\nReason:\tstack__c[%zu] != "
	     "stack__jazz[%zu] ==> %x != %x\n",
	     i, i, sk_c[i], sk_jazz[i]);
    }
  }
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  return res;
}

int test_MMT_th_set_fin_0(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  memcpy(sk_c, sk, MMT_BYTES);
  memcpy(sk_jazz, sk, MMT_BYTES);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  (state_c.treehash + TH_INDEX)->fin = 0;
  MMT_th_set_fin_0_jazz(sk_jazz, TH_INDEX);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < MMT_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_th_set_fin_0.\nReason:\tsk_c[%zu] != "
	     "sk_jazz[%zu] ==> %x != %x\n",
	     i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_MMT_th_set_fin_1(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  memcpy(sk_c, sk, MMT_BYTES);
  memcpy(sk_jazz, sk, MMT_BYTES);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  (state_c.treehash + TH_INDEX)->fin = 1;
  MMT_th_set_fin_1_jazz(sk_jazz, TH_INDEX);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < MMT_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_th_set_fin_1.\nReason:\tsk_c[%zu] != "
	     "sk_jazz[%zu] ==> %x != %x\n",
	     i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_MMT_th_set_idx(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  uint64_t idx;

  memcpy(sk_c, sk, MMT_BYTES);
  memcpy(sk_jazz, sk, MMT_BYTES);

  random_test_64bit_blocks(&idx, 1);
  idx &= (1 << PYR_FULL_HEIGHT) - 1;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  (state_c.treehash + TH_INDEX)->idx = idx;
  MMT_th_set_idx_jazz(sk_jazz, idx, TH_INDEX);

  /* serialize to write non-pointers */
  state_serialize(sk_c, &state_c, STATE_TYPE);
  for (i = 0; i < MMT_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_th_set_idx.\nReason:\tsk_c[%zu] != "
	     "sk_jazz[%zu] ==> %x != %x\n",
	     i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_MMT_th_set_sda(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char node[PYR_N];

  memcpy(sk_c, sk, MMT_BYTES);
  memcpy(sk_jazz, sk, MMT_BYTES);

  random_test_bytes(node, PYR_N);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  memcpy((state_c.treehash + TH_INDEX)->ref_active, node, PYR_N);
  MMT_th_set_sda_jazz(sk_jazz, node, TH_INDEX);

  for (i = 0; i < MMT_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_th_set_sda.\nReason:\tsk_c[%zu] != "
	     "sk_jazz[%zu] ==> %x != %x\n",
	     i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int test_MMT_get_auth_node(const unsigned char *sk) {
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
  MMT_get_auth_node_jazz(node_jazz, sk, TH_INDEX);

  for (i = 0; i < PYR_N; ++i) {
    if (node_c[i] != node_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_get_auth_node.\nReason:\tnode_c[%zu] != "
	     "node_jazz[%zu] ==> %x != %x\n",
	     i, i, node_c[i], node_jazz[i]);
    }
  }

  return res;
}

int test_MMT_set_auth_node(const unsigned char *sk) {
  int res = 0;
  size_t i;
  unsigned char sk_c[MMT_BYTES];
  unsigned char sk_jazz[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];

  unsigned char node[PYR_N];

  memcpy(sk_c, sk, MMT_BYTES);
  memcpy(sk_jazz, sk, MMT_BYTES);

  random_test_bytes(node, PYR_N);

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  state_deserialize(sk_c, &state_c, STATE_TYPE);

  memcpy(state_c.auth + (AU_INDEX * PYR_N), node, PYR_N);
  MMT_set_auth_node_jazz(sk_jazz, node, AU_INDEX);

  for (i = 0; i < MMT_BYTES; ++i) {
    if (sk_c[i] != sk_jazz[i]) {
      res = -1;
      printf(
	     "[!] Unit test "
	     "failed.\nFunction:\tMMT_set_auth_node.\nReason:\tsk_c[%zu] != "
	     "sk_jazz[%zu] ==> %x != %x\n",
	     i, i, sk_c[i], sk_jazz[i]);
    }
  }

  return res;
}

int main() {
  int res = 0;
  unsigned char sk_c[MMT_BYTES];
  state_type state_c;
  stack_type stack_c[1];
  treehash_type treehash_c[32];
  unsigned char push_node[PYR_N];
  unsigned char push_node_height;

  state_c.stack = stack_c;
  state_c.treehash = treehash_c;
  random_test_bytes(sk_c, MMT_BYTES);
  random_test_bytes(&push_node_height, 1);
  random_test_bytes(push_node, PYR_N);

  res |= test_MMT_th_get_fin(sk_c);
  res |= test_MMT_th_get_idx(sk_c);
  res |= test_MMT_th_get_sda(sk_c);

  res |= test_MMT_th_set_fin_0(sk_c);
  res |= test_MMT_th_set_fin_1(sk_c);
  res |= test_MMT_th_set_idx(sk_c);
  res |= test_MMT_th_set_sda(sk_c);

  res |= test_MMT_st_get_size(sk_c);
  res |= test_MMT_st_get_height(sk_c);
  res |= test_MMT_st_set_size(sk_c);
  res |= test_MMT_st_set_height(sk_c);

  /* Ensure non-empty stack for pop test. */
  state_deserialize(sk_c, &state_c, STATE_TYPE);
  state_c.stack->size = 0;
  stack_push(state_c.stack, push_node, push_node_height);
  state_serialize(sk_c, &state_c, STATE_TYPE);
  /* Stack size is now 1. */
  
  res |= test_MMT_stack_pop(sk_c);

  /* Ensure empty stack for push test. */
  state_deserialize(sk_c, &state_c, STATE_TYPE);
  state_c.stack->size = 0;
  state_serialize(sk_c, &state_c, STATE_TYPE);
  /* Stack size is now 0. */
  
  res |= test_MMT_stack_push(sk_c);

  res |= test_MMT_get_auth_node(sk_c);
  res |= test_MMT_set_auth_node(sk_c);

  if (res) {
    printf("Tests failed.\n");
  }

  return 0;
}

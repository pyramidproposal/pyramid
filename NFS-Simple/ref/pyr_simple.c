#include "pyr_simple.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "address.h"
#include "convert.h"
#include "format.h"
#include "hash.h"
#include "params.h"
#include "thash.h"
#include "wots.h"

// Treehash instance
typedef struct {
  unsigned char *node;
  unsigned char fin;
  uint64_t idx;
} treehash_type;
// Stack
typedef struct {
  unsigned char size;
  unsigned char *node;
  unsigned char *nodeheight;
} stack_type;
// bds/mmt state
typedef struct {
  unsigned char *auth;
  stack_type *stack;
  treehash_type *treehash;
  unsigned char *keep;
} state_type;
// Get height of the first left parent 
static uint32_t get_tau(const uint32_t idx_leaf) {
  uint32_t i = 0;
  while (((idx_leaf >> i) & 1) == 1 && i < PYR_TREE_HEIGHT) {
    i++;
  }
  return i;
}
// Regular pop operation on stack
static void stack_pop(stack_type *stack, unsigned char *pop_node) {
  stack->size--;
  memcpy(pop_node, stack->node + stack->size * PYR_N, PYR_N);
  memset(stack->node + stack->size * PYR_N, 0, PYR_N);
  stack->nodeheight[stack->size] = 0;
}
// Regular push operation on stack
static void stack_push(stack_type *stack, unsigned char *push_node,
                       unsigned char push_nodeheight) {
  memcpy(stack->node + stack->size * PYR_N, push_node, PYR_N);
  stack->nodeheight[stack->size] = push_nodeheight;
  stack->size++;
}
// Given leaf index i and height of root node s, computes the root node using
// treehash algorithm.
// See RFC8391.
static void tree_hash(unsigned char *root, const unsigned char *sk, uint32_t s,
                      uint32_t t, const uint32_t subtree_addr[8]) {
  const unsigned char *const sk_psd = sk + SK_PSD;
  const unsigned char *const sk_ssd = sk + SK_SSD;
  uint32_t i;
  stack_type stack;
  unsigned char stack_node[(t + 1) * PYR_N];
  unsigned char stack_height[t + 1];
  unsigned char node[PYR_N];
  unsigned char thash_h_in[2 * PYR_N];
  uint32_t ots_addr[8] = {0};
  uint32_t ltree_addr[8] = {0};
  uint32_t node_addr[8] = {0};

  // 1 Init
  // 1.1. Init 3 addresses.
  copy_subtree_addr(ots_addr, subtree_addr);
  copy_subtree_addr(ltree_addr, subtree_addr);
  copy_subtree_addr(node_addr, subtree_addr);
  set_type(ots_addr, PYR_ADDR_TYPE_WOTS);
  set_type(ltree_addr, PYR_ADDR_TYPE_WOTSPK);
  set_type(node_addr, PYR_ADDR_TYPE_HASHTREE);
  // 1.2. Init stack.
  stack.size = 0;
  stack.node = stack_node;
  stack.nodeheight = stack_height;
  // 2 Compute root.
  for (i = 0; i < (uint32_t)(1 << t); i++) {
    // 2.1. Compute a leaf.
    set_keypair_addr(ltree_addr, s + i);
    set_keypair_addr(ots_addr, s + i);
    gen_leaf_wots(node, sk_ssd, sk_psd, ltree_addr, ots_addr);

    // 2.2. Repeat t_hash while node and top of the stack have the same height.
    set_tree_height(node_addr, 0);
    set_tree_index(node_addr, i + s);
    while (stack.size > 0 &&
           stack.nodeheight[stack.size - 1] == get_tree_height(node_addr)) {
      set_tree_index(node_addr, (get_tree_index(node_addr) - 1) >> 1);
      stack_pop(&stack, thash_h_in);
      memcpy(thash_h_in + PYR_N, node, PYR_N);
      thash(node, thash_h_in, 2, sk_psd, node_addr);
      set_tree_height(node_addr, get_tree_height(node_addr) + 1);
    }
    stack_push(&stack, node, get_tree_height(node_addr));
  }
  stack_pop(&stack, root);
}
// Given leaf index i, generate XMSS signature from state.
// WOTS signature generation + copy authentication path from state.
static void tree_sig(unsigned char *sig,  // output
                     const unsigned char *M, const unsigned char *sk_ssd,
                     const unsigned char *sk_psd, const uint32_t idx_leaf,
                     const uint32_t subtree_addr[8],
                     const unsigned char *auth_path) {
  uint32_t wots_addr[8] = {0};

  // 1 Init
  // 1.1. Init address.
  copy_subtree_addr(wots_addr, subtree_addr);
  set_type(wots_addr, PYR_ADDR_TYPE_WOTS);
  // 2. Generate WOTS signature.
  set_keypair_addr(wots_addr, idx_leaf);
  wots_sign(sig, M, sk_ssd, sk_psd, wots_addr);
  // 3. Copy auth to sig.
  memcpy(sig + PYR_WOTS_BYTES, auth_path, PYR_TREE_HEIGHT * PYR_N);
}
// Given leaf index i and stack, generate leaf and compute hash value of leaf
// and top node while leaf (or hash value) and top node have the same height.
// Afterword, push the result on the stack.
static void update_treehash(const unsigned char *sk,
                            const uint32_t target_height, state_type *state,
                            const uint32_t layer) {
  const unsigned char *const sk_psd = sk + SK_PSD;
  unsigned char node[2 * PYR_N];
  unsigned char thash_h_in[2 * PYR_N];
  uint32_t addr[8] = {0};
  uint64_t idx_tree = (state->treehash + target_height)->idx >> PYR_TREE_HEIGHT;
  uint32_t idx_leaf = (uint32_t)((state->treehash + target_height)->idx &
                                 ((1ULL << PYR_TREE_HEIGHT) - 1ULL));

  // 1 Init
  // 1.1. Init an address.
  set_layer_addr(addr, layer);
  set_tree_addr(addr, idx_tree);
  set_type(addr, PYR_ADDR_TYPE_HASHTREE);
  set_tree_height(addr, 0);
  set_tree_index(addr, idx_leaf);
  // 2. Generate a leaf
  tree_hash(node, sk, idx_leaf, 0, addr);
  while (state->stack->size > 0 &&
         state->stack->nodeheight[state->stack->size - 1] ==
             get_tree_height(addr) &&
         get_tree_height(addr) < target_height) {
    set_tree_index(addr, (get_tree_index(addr) - 1) >> 1);
    stack_pop(state->stack, thash_h_in);
    memcpy(thash_h_in + PYR_N, node, PYR_N);
    thash(node, thash_h_in, 2, sk_psd, addr);
    set_tree_height(addr, get_tree_height(addr) + 1);
  }
  // 3. Node does not reach target height, push node on stack and increase idx
  // by 1.
  if (get_tree_height(addr) < target_height) {
    stack_push(state->stack, node, get_tree_height(addr));
    (state->treehash + target_height)->idx++;
    // 4. Node reaches target height, treehash instance is completed.
  } else {
    (state->treehash + target_height)->fin = 1;
    (state->treehash + target_height)->idx = 0;
    if (layer == 0) {
      memcpy((state->treehash + target_height)->node, node, PYR_N);
    } else {
      stack_push(state->stack, node, get_tree_height(addr));
    }
  }
}

// Given XMSS signature, return root of XMSS tree.
// See RFC8391.
void xmss_root_from_sig(unsigned char *root,  // output
                        const uint32_t idx_leaf, const unsigned char *sig,
                        const unsigned char *msg,  // must be hashed
                        const unsigned char *sk_psd,
                        const uint32_t subtree_addr[8]) {
  unsigned char wots_pk[PYR_WOTS_PK_BYTES];
  unsigned char leaf[PYR_N];
  uint32_t ots_addr[8] = {0};
  uint32_t ltree_addr[8] = {0};
  uint32_t node_addr[8] = {0};
  unsigned char thash_h_in[2 * PYR_N];
  uint32_t k;

  // 1. Init
  // 1.1. Init 3 addresses.
  copy_subtree_addr(ots_addr, subtree_addr);
  copy_subtree_addr(ltree_addr, subtree_addr);
  copy_subtree_addr(node_addr, subtree_addr);
  set_type(ots_addr, PYR_ADDR_TYPE_WOTS);
  set_type(ltree_addr, PYR_ADDR_TYPE_WOTSPK);
  set_type(node_addr, PYR_ADDR_TYPE_HASHTREE);
  // 2. Compute leaf (WOTS public key).
  set_keypair_addr(ots_addr, idx_leaf);
  wots_pk_from_sig(wots_pk, sig, msg, sk_psd, ots_addr);
  set_keypair_addr(ltree_addr, idx_leaf);
  thash(leaf, wots_pk, PYR_WOTS_LEN, sk_psd, ltree_addr);
  // 3. Traversal from leaf using auth in signature.
  set_tree_index(node_addr, idx_leaf);
  memcpy(root, leaf, PYR_N);
  for (k = 0; k < PYR_TREE_HEIGHT; k++) {
    set_tree_height(node_addr, k);
    if (!((idx_leaf >> k) & 1)) {
      set_tree_index(node_addr, get_tree_index(node_addr) >> 1);
      memcpy(thash_h_in, root, PYR_N);
      memcpy(thash_h_in + PYR_N, sig + PYR_WOTS_BYTES + (k * PYR_N), PYR_N);
    } else {
      set_tree_index(node_addr, (get_tree_index(node_addr) - 1) >> 1);
      memcpy(thash_h_in, sig + PYR_WOTS_BYTES + (k * PYR_N), PYR_N);
      memcpy(thash_h_in + PYR_N, root, PYR_N);
    }
    thash(root, thash_h_in, 2, sk_psd, node_addr);
  }
}
// Generate bds state from secret key.
static void bds_stateGen(unsigned char *sk, state_type *state) {
  const unsigned char *const sk_psd = sk + SK_PSD;
  unsigned int i;
  unsigned char thash_h_in[2 * PYR_N];
  uint32_t addr[8] = {0};

  // 1.Init
  // 1.1. Init address.
  set_type(addr, PYR_ADDR_TYPE_HASHTREE);
  // 2.Generate authentication path
  // 2.1. auth[0]
  tree_hash(state->auth, sk, 1, 0, addr);
  // 2.2. auth[1]...auth[h/d-1] and treehash[0]...treehash[h/d-1]
  for (i = 0; i < PYR_TREE_HEIGHT - 1; i++) {
    // 2.2.1. treehash[i]
    tree_hash((state->treehash + i)->node, sk, 3 * (1 << i), i, addr);
    (state->treehash + i)->fin = 1;
    // 2.2.2. auth[i]
    tree_hash(thash_h_in, sk, 2 * (1 << i), i, addr);
    memcpy(thash_h_in + PYR_N, (state->treehash + i)->node, PYR_N);
    set_tree_height(addr, i);
    set_tree_index(addr, 1);
    thash(state->auth + (i + 1) * PYR_N, thash_h_in, 2, sk_psd, addr);
  }
  // 3. Init other entries.
  state->stack->size = 0;
  memset(state->stack->node, 0, (PYR_TREE_HEIGHT - 1) * PYR_N);
  memset(state->stack->nodeheight, 0, PYR_TREE_HEIGHT - 1);
  memset(state->keep, 0, (PYR_TREE_HEIGHT >> 1) * PYR_N);
  // 4. Initialization of highest instance of h/d - 1 in the next XMSS tree.
  if (PYR_D > 1) {
#ifdef FAST_KEYGEN
    // Execute h/(2d)+1 treehash upadates on highest treehash instance.
    // This computation prevents the other instances from popping nodes of this
    // instance.
    (state->treehash + PYR_TREE_HEIGHT - 1)->fin = 0;
    (state->treehash + PYR_TREE_HEIGHT - 1)->idx =
        3 * (1 << (PYR_TREE_HEIGHT - 1));
    for (i = 0; i < ((PYR_TREE_HEIGHT + 1) >> 1) + 1 &&
                (state->treehash + PYR_TREE_HEIGHT - 1)->fin == 0;
         i++) {
      update_treehash(sk, PYR_TREE_HEIGHT - 1, state, 0);
    }
#else
    // Highest treehash instance generates its target node.
    // This requires 2^{h/d-1} leaf computations. If you want to avoid it
    // predefine FASR_KEYGEN.
    (state->treehash + PYR_TREE_HEIGHT - 1)->fin = 1;
    set_tree_addr(addr, 1);
    tree_hash((state->treehash + PYR_TREE_HEIGHT - 1)->node, sk,
              3 * (1 << (PYR_TREE_HEIGHT - 1)), PYR_TREE_HEIGHT - 1, addr);
#endif
  } else {
    // The highest instance of h/d - 1  has no meaning in XMSS. Kill the
    // functionality.
    (state->treehash + PYR_TREE_HEIGHT - 1)->fin = 1;
  }
}
// Generate mmt state from secret key.
static void mmt_stateGen(unsigned char *sk, state_type *state, uint32_t layer) {
  unsigned int i;
  uint32_t addr[8] = {0};

  // 1.Init
  // 1.1. Set address.
  set_type(addr, PYR_ADDR_TYPE_HASHTREE);
  set_layer_addr(addr, layer);
  // 2. Generate authentication path
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    tree_hash(state->auth + i * PYR_N, sk, (1 << i), i, addr);
    (state->treehash + i)->fin = 1;
    (state->treehash + i)->idx = 0;
  }
  // 3. Init treehash instance of 0.
  state->treehash->fin = 0;
  // 4. Init other entries.
  state->stack->size = 0;
  memset(state->stack->node, 0, PYR_TREE_HEIGHT * PYR_N);
  memset(state->stack->nodeheight, 0, PYR_TREE_HEIGHT);
}
// Update bds state after signature generation.
static void bds_state_update(unsigned char *sk, state_type *state) {
  const unsigned char *const sk_idx = sk + SK_IDX;
  const unsigned char *const sk_psd = sk + SK_PSD;
  uint32_t i, j;
  uint64_t idx_tmp, idx_tree, idx_sig = 0;
  uint32_t idx_leaf;
  unsigned char thash_h_in[2 * PYR_N];
  uint32_t addr[8] = {0};
  uint32_t tau;

  // 1.Init
  // 1.1. Init indices.
  idx_sig = bytes_to_ull(sk_idx, PYR_IDX_BYTES);
  idx_tree = (uint64_t)(idx_sig >> PYR_TREE_HEIGHT);
  idx_leaf = (uint32_t)((idx_sig & ((1ULL << PYR_TREE_HEIGHT) - 1ULL)));
  // 1.2. Set address.
  set_tree_addr(addr, idx_tree);
  set_type(addr, PYR_ADDR_TYPE_HASHTREE);
  // 2. Compute tau, i.e., height of the first left parent of leafnode.
  tau = get_tau(idx_leaf);
  // 3. Backup keep[(tau-1)>>1]
  if (tau > 0) {
    memcpy(thash_h_in, state->auth + (tau - 1) * PYR_N, PYR_N);
    memcpy(thash_h_in + PYR_N, state->keep + ((tau - 1) >> 1) * PYR_N, PYR_N);
  }
  // 4. Keep auth[tau] in keep.
  if (!((idx_leaf >> (tau + 1)) & 1) && (tau < PYR_TREE_HEIGHT - 1)) {
    memcpy(state->keep + (tau >> 1) * PYR_N, state->auth + tau * PYR_N, PYR_N);
  }
  // 5. if tau = 0, generate auth[0].
  if (tau == 0) {
    tree_hash(state->auth, sk, idx_leaf, 0, addr);
  }
  // 6. if tau > 0,
  else {
    // 6.1. Compute a hash value of keep[(tau-1)>>1] and auth[tau-1] to get new
    // auth[tau].
    if (tau < PYR_TREE_HEIGHT) {
      set_tree_height(addr, tau - 1);
      set_tree_index(addr, idx_leaf >> tau);
      thash(state->auth + tau * PYR_N, thash_h_in, 2, sk_psd, addr);
    }
    // 6.2. Copy treehash.node to auth and initialize copied treehash instances
    // from 0 to tau-1.
    for (j = 0; j < tau; j++) {
      memcpy(state->auth + j * PYR_N, (state->treehash + j)->node, PYR_N);
      memset((state->treehash + j)->node, 0, PYR_N);
      idx_tmp = ((uint64_t)idx_sig + 1ULL + 3ULL * (1ULL << j));
      if (idx_tmp < (uint64_t)(1ULL << PYR_FULL_HEIGHT)) {
        (state->treehash + j)->idx = idx_tmp;
        (state->treehash + j)->fin = 0;
      }
    }
  }
#ifdef SAFE_MODE
  // Check if nodes are correctly pushed with respect to their heights.
  for (i = 0; i < state->stack->size; ++i) {
    if (tau > 1 && state->stack->nodeheight[i] < tau - 1) {
      exit(0);
    }
  }
#endif
  // 7. Treehash updates.
  for (i = 0; i<(PYR_TREE_HEIGHT + 1) >> 1; i++) {
    // 7.1. Get the lowest unfinished treehash instance.
    j = 0;
    while ((state->treehash + j)->fin == 1 && j < PYR_TREE_HEIGHT) {
      j++;
    }
    // 7.2. Update treehash[j] and stack if needed.
    if (j < PYR_TREE_HEIGHT) {
      update_treehash(sk, j, state, 0);
    } else {
      i = (PYR_TREE_HEIGHT + 1) >> 1;
    }
  }
}
// Update mmt state after signature generation.
static void mmt_state_update(unsigned char *sk, state_type *state,
                             uint32_t layer) {
  const unsigned char *const sk_idx = sk + SK_IDX;
  uint32_t i, j;
  uint64_t idx_next, idx_sig = 0;
  uint32_t idx_leaf;
  uint32_t addr[8] = {0};
  uint32_t tau1, tau2;

  // 1.Init
  // 1.1. Init indices.
  idx_sig = bytes_to_ull(sk_idx, PYR_IDX_BYTES);
  idx_next = idx_sig + 1ULL;
  idx_sig = (uint64_t)(idx_sig >> (layer * PYR_TREE_HEIGHT));
  idx_leaf = (uint32_t)((idx_sig & ((1ULL << PYR_TREE_HEIGHT) - 1ULL)));
  set_type(addr, PYR_ADDR_TYPE_HASHTREE);
  // 2. Update authentication path and initialize treehash instaces.
  if ((idx_next & ((1ULL << (layer * PYR_TREE_HEIGHT)) - 1ULL)) == 0) {
    tau1 = get_tau(idx_leaf);
    for (i = 0; i < (tau1 + 1) && i < PYR_TREE_HEIGHT; i++) {
      stack_pop(state->stack, state->auth + i * PYR_N);
    }
    tau2 = get_tau(idx_leaf + 1);
    for (i = 0; i < (tau2 + 1) && i < PYR_TREE_HEIGHT; i++) {
      if (i == tau2) {
        (state->treehash + i)->idx = idx_sig + 2ULL - (1ULL << i);
        (state->treehash + i)->fin = 0;
      } else if ((idx_sig + 2ULL + (1ULL << i)) <
                 (uint64_t)(1ULL
                            << (PYR_FULL_HEIGHT - (layer * PYR_TREE_HEIGHT)))) {
        (state->treehash + i)->idx = (uint64_t)(idx_sig + 2ULL + (1ULL << i));
        (state->treehash + i)->fin = 0;
      }
    }
    // 3. Treehash update (only once).
  } else if ((idx_next & ((1ULL << ((layer - 1) * PYR_TREE_HEIGHT)) - 1ULL)) ==
             0) {
    // 3.1. Get the highest unfinished treehash instance.
    j = PYR_TREE_HEIGHT;
    while (j > 0 && (state->treehash + j - 1)->fin == 1) {
      j--;
    }
    // 3.2. Update treehash[j] and stack if needed.
    if (j > 0) {
      i = j - 1;
      update_treehash(sk, i, state, layer);
    }
  }
}
static void state_serialize(unsigned char *sk, state_type *state,
                            unsigned int layer) {
  unsigned int i;

  // auth
  sk += PYR_TREE_HEIGHT * PYR_N;
  // stack
  sk[0] = state->stack->size;
  sk++;
  if (layer == 0) {
    sk += (PYR_TREE_HEIGHT - 1) * (PYR_N + 1);
  } else {
    sk += PYR_TREE_HEIGHT * (PYR_N + 1);
  }
  // treehash
  if (layer == 0) {
    sk += PYR_TREE_HEIGHT * PYR_N;
  }
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    sk[0] = (state->treehash + i)->fin;
    sk += 1;
  }
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    ull_to_bytes(sk, ((PYR_FULL_HEIGHT + 7) >> 3), (state->treehash + i)->idx);
    sk += ((PYR_FULL_HEIGHT + 7) >> 3);
  }
}

static void state_deserialize(unsigned char *sk, state_type *state,
                              unsigned int layer) {
  unsigned int i;

  // auth
  state->auth = sk;
  sk += PYR_TREE_HEIGHT * PYR_N;
  // stack
  state->stack->size = sk[0];
  sk++;
  if (layer == 0) {
    state->stack->node = sk;
    sk += (PYR_TREE_HEIGHT - 1) * PYR_N;
    state->stack->nodeheight = sk;
    sk += PYR_TREE_HEIGHT - 1;
  } else {
    state->stack->node = sk;
    sk += PYR_TREE_HEIGHT * PYR_N;
    state->stack->nodeheight = sk;
    sk += PYR_TREE_HEIGHT;
  }
  // treehash
  if (layer == 0) {
    for (i = 0; i < PYR_TREE_HEIGHT; i++) {
      (state->treehash + i)->node = sk;
      sk += PYR_N;
    }
  }
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    (state->treehash + i)->fin = sk[0];
    sk += 1;
  }
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    (state->treehash + i)->idx = bytes_to_ull(sk, ((PYR_FULL_HEIGHT + 7) >> 3));
    sk += ((PYR_FULL_HEIGHT + 7) >> 3);
  }
  if (layer == 0) {
    state->keep = sk;
  }
}

static const unsigned char *sk_get_auth(const unsigned char *sk,
                                        unsigned int layer) {
  const unsigned char *auth_path_layer = sk;

  auth_path_layer += PYR_SK_CORE_BYTES;
  if (layer) {
    auth_path_layer += BDS_BYTES;
    auth_path_layer += (layer - 1) * MMT_BYTES;
  }
  return auth_path_layer;
}

int xmssmt_finalize_sk(unsigned char *root, unsigned char *sk) {
  const unsigned char *const sk_psd = sk + SK_PSD;
  unsigned char *sk_simple = sk + PYR_SK_CORE_BYTES;
  uint32_t addr[8] = {0};
  unsigned int i;
  unsigned char leaf[PYR_N];
  unsigned char thash_h_in[2 * PYR_N];
  state_type state;
  stack_type stack[1];
  treehash_type treehash[32];

  // 1.4. Memmory allocation of members of state.
  state.stack = stack;
  state.treehash = treehash;
  // 2. Generate state.
  // 2.1. Generate BDS state.
  state_deserialize(sk_simple, &state, 0);
  bds_stateGen(sk, &state);
  state_serialize(sk_simple, &state, 0);
  sk_simple += BDS_BYTES;
  // 2.2. Generate MMT states.
  for (i = 1; i < PYR_D; i++) {
    state_deserialize(sk_simple, &state, i);
    mmt_stateGen(sk, &state, i);
    state_serialize(sk_simple, &state, i);
    sk_simple += MMT_BYTES;
  }
  // 3. Generate root of top XMSS tree.
  set_layer_addr(addr, PYR_D - 1);
  tree_hash(leaf, sk, 0, 0, addr);
  set_type(addr, PYR_ADDR_TYPE_HASHTREE);
  set_tree_index(addr, 0);
  memcpy(root, leaf, PYR_N);
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    set_tree_height(addr, i);
    memcpy(thash_h_in, root, PYR_N);
    memcpy(thash_h_in + PYR_N, state.auth + (i * PYR_N), PYR_N);
    thash(root, thash_h_in, 2, sk_psd, addr);
  }

  return 0;
}

int xmssmt_ht_sign(const unsigned char *sk, unsigned char *sig, size_t *sig_len,
                   const unsigned char *mhash) {
  const unsigned char *const sk_idx = sk + SK_IDX;
  const unsigned char *const sk_ssd = sk + SK_SSD;
  const unsigned char *const sk_psd = sk + SK_PSD;
  const unsigned char *auth_path;
  uint32_t addr[8] = {0};
  unsigned char root[PYR_N];
  uint64_t idx_tree;
  uint32_t idx_leaf;
  uint64_t i;

  // Offset to HyperTree signature part
  sig += SIG_HTS;

  // 1. Init
  // 1.1. Get indicises.
  idx_tree = bytes_to_ull(sk_idx, PYR_IDX_BYTES);

  // 5. Signature generation in each layer
  for (i = 0; i < PYR_D; i++) {
    if (i == 0) {
      memcpy(root, mhash, PYR_N);
    } else {  // 5.2.1. Compute root of XMSS tree a layer below.
      xmss_root_from_sig(root, idx_leaf, sig - SIG_XMSS_BYTES, root, sk_psd,
                         addr);
    }

    idx_leaf = (idx_tree & ((1 << PYR_TREE_HEIGHT) - 1));
    idx_tree >>= PYR_TREE_HEIGHT;
    set_layer_addr(addr, i);
    set_tree_addr(addr, idx_tree);

    // 5.2.2. Sign root.
    auth_path = sk_get_auth(sk, i);
    tree_sig(sig, root, sk_ssd, sk_psd, idx_leaf, addr, auth_path);

    sig += SIG_XMSS_BYTES;
    *sig_len += SIG_XMSS_BYTES;
  }

  return 0;
}

int xmssmt_update_state(unsigned char *sk) {
  unsigned char *sk_simple = sk + PYR_SK_CORE_BYTES;
  state_type state;
  stack_type stack[1];
  treehash_type treehash[32];
  unsigned int i;

  state.stack = stack;
  state.treehash = treehash;

  // 1 BDS state update.
  state_deserialize(sk_simple, &state, 0);
  bds_state_update(sk, &state);
  state_serialize(sk_simple, &state, 0);
  sk_simple += BDS_BYTES;

  // 0+ MMT state update(s).
  for (i = 1; i < PYR_D; i++) {
    state_deserialize(sk_simple, &state, i);
    mmt_state_update(sk, &state, i);
    state_serialize(sk_simple, &state, i);
    sk_simple += MMT_BYTES;
  }

  return 0;
}

// Key finalization of XMSS.
int xmss_finalize_sk(unsigned char *root, unsigned char *sk) {
  return xmssmt_finalize_sk(root, sk);
}

// Signature generation of XMSS.
int xmss_t_sign(const unsigned char *sk, unsigned char *sig, size_t *sig_len,
                const unsigned char *mhash) {
  return xmssmt_ht_sign(sk, sig, sig_len, mhash);
}

// State update of XMSS.
int xmss_update_state(unsigned char *sk) { return xmssmt_update_state(sk); }

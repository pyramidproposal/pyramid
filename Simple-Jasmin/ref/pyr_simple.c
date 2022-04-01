#include "pyr_simple.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "address.h"
#include "convert.h"
#include "format.h"
#include "hash.h"
#include "merkle.h"
#include "params.h"
#include "thash.h"
#include "wots.h"

/*
  This file is based on "xmss_simple: Simple and Memory-efficient XMSS^MT",
  by Haruhisa Kosuge and Hidema Tanaka.
  Original code: https://github.com/HaruCrypto54/xmss_simple on Sep 15, 2021.
 */

/*
  Please keep in mind that parameters such as
  "const unsigned char sk[PYR_SK_BYTES], state_type *state"
  only imply that the sk region is not overwritten through the sk pointer.
*/

/* For throwing an overflow error when generating trees ahead of time. */
#if PYR_FULL_HEIGHT > 64
#define IDX_BITS ((64 / PYR_TREE_HEIGHT) * PYR_TREE_HEIGHT)
#else
#define IDX_BITS PYR_FULL_HEIGHT
#endif

/* Get height of the first left parent. Preserved unoptimized implementation for
 * clarity. */
uint32_t get_tau(const uint32_t idx_leaf) {
  uint32_t i = 0;
  while (((idx_leaf >> i) & 1) == 1 && i < PYR_TREE_HEIGHT) {
    i++;
  }
  return i;
}

/* Pop a from stack into pop_node. */
void stack_pop(stack_type *stack, unsigned char *pop_node) {
  unsigned char *top;
  stack->size--;
  top = stack->node + stack->size * PYR_N;
  memcpy(pop_node, top, PYR_N);
  memset(top, 0, PYR_N);
  stack->nodeheight[stack->size] = 0;
}

/* Push a node push_node of height push_node_height on the stack. */
void stack_push(stack_type *stack, const unsigned char *push_node,
                unsigned char push_nodeheight) {
  memcpy(stack->node + stack->size * PYR_N, push_node, PYR_N);
  stack->nodeheight[stack->size] = push_nodeheight;
  stack->size++;
}

/**
 * @brief Treehash algorithm using FSG.
 * @details Performs treehash for (sub)trees, using fsg to forward FSG
 * references, which are used to generate WOTS seeds. The instance (s=0,
 * t=PYR_TREE_HEIGTH) is commonly referred to as "Treehash".
 * @param[out] root The root of the subtree.
 * @param[out] next_ref The first FSG reference after the subtree to generate.
 * @param[in] sk_ssd The first FSG reference for the subtree to generate.
 * @param[in] sk_psd The public seed.
 * @param[in] s The index of the first leaf node within the subtree to
 * generate.
 * @param[in] t The height of the root to generate.
 * @param[in] subtree_addr The address of the tree that the subtree to generate
 * is located in. [layer address] and [tree address] should be set.
 */
void tree_hash(unsigned char root[PYR_N], unsigned char next_ref[PYR_N],
               const unsigned char fsg_ref[PYR_N],
               const unsigned char sk_psd[PYR_N], uint32_t s, uint32_t t,
               const uint32_t subtree_addr[8]) {
  uint32_t wots_addr[8] = {0};
  uint32_t wotspk_addr[8] = {0};
  uint32_t hashtree_addr[8] = {0};
  unsigned char stack_node[(t + 1) * PYR_N];
  unsigned char stack_height[t + 1];
  unsigned char node[PYR_N];
  unsigned char thash_h_in[2 * PYR_N];
  unsigned char R[PYR_N]; /* FSG Reference */
  unsigned char S[PYR_N]; /* WOTS Seed */
  stack_type stack;
  uint32_t i, j;

  /* Copy the initial reference, so we can modity it. */
  memcpy(R, fsg_ref, PYR_N);

  copy_subtree_addr(wots_addr, subtree_addr);
  copy_subtree_addr(wotspk_addr, subtree_addr);
  copy_subtree_addr(hashtree_addr, subtree_addr);
  set_type(wots_addr, PYR_ADDR_TYPE_WOTS);
  set_type(wotspk_addr, PYR_ADDR_TYPE_WOTSPK);
  set_type(hashtree_addr, PYR_ADDR_TYPE_HASHTREE);

  stack.size = 0;
  stack.node = stack_node;
  stack.nodeheight = stack_height;

  /* Compute the root at height t. */
  for (i = 0; i < (uint32_t)(1 << t); i++) {
    /* Compute the tree at offset s. */
    set_keypair_addr(wotspk_addr, s + i);
    set_keypair_addr(wots_addr, s + i);

    /* Generate the next WOTS seed and FSG reference.  */
    if (i == (uint32_t)(1 << t) - 1) {
      fsg(S, next_ref, R, sk_psd, wots_addr);
    } else {
      fsg(S, R, R, sk_psd, wots_addr);
    }
    gen_leaf_wots(node, S, sk_psd, wotspk_addr, wots_addr);

    /* Combine whilst we have nodes of the same height > 0. */
    j = 0;
    while (stack.size > 0 && stack.nodeheight[stack.size - 1] == j) {
      set_tree_index(hashtree_addr, (i + s) >> (j + 1));
      set_tree_height(hashtree_addr, j);
      stack_pop(&stack, thash_h_in);
      memcpy(thash_h_in + PYR_N, node, PYR_N);
      thash(node, thash_h_in, 2, sk_psd, hashtree_addr);
      j++;
    }

    /* Store the result on the stack. */
    stack_push(&stack, node, j);
  }

  /* Return the root node for (s,t) in root. */
  stack_pop(&stack, root);
}

/**
 * @brief Given leaf index i, generate XMSS signature from state.
 * @details WOTS signature generation + copy authentication path from state to
 * sig.
 * @param[out] sig The WOTS signature, followed by the authentication path.
 * @param[in] M The compressed message to generate the WOTS signature for.
 * @param[in] wots_seed The WOTS seed to expand.
 * @param[in] sk_psd The public seed.
 * @param[in] idx_leaf The index of the leaf that is being signed with.
 * @param[in] subtree_addr The address of the tree that the leaf to sign with is
 * located in. [layer address] and [tree address] should be set.
 * @param[in] auth_path The auth path to sign with.
 */
void tree_sig(unsigned char sig[SIG_XMSS_BYTES], const unsigned char M[PYR_N],
              const unsigned char wots_seed[PYR_N],
              const unsigned char sk_psd[PYR_N], const uint32_t idx_leaf,
              const uint32_t subtree_addr[8],
              const unsigned char auth_path[PYR_TREE_HEIGHT * PYR_N]) {
  uint32_t wots_addr[8] = {0};

  copy_subtree_addr(wots_addr, subtree_addr);
  set_type(wots_addr, PYR_ADDR_TYPE_WOTS);

  /* Generate WOTS signature. */
  set_keypair_addr(wots_addr, idx_leaf);
  wots_sign(sig, M, wots_seed, sk_psd, wots_addr);

  /* Copy the authentication path into the signature. */
  memcpy(sig + PYR_WOTS_BYTES, auth_path, PYR_TREE_HEIGHT * PYR_N);
}

/**
 * @brief Instance version of the Treehash algorithm.
 * @details Compute one leaf for the Treehash instance that operates on tree
 * height target_height. The Treehash instance shares a stack which is accessed
 * through state. State is a BDS/MMT state that operates on hypertree layer
 * layer.
 * @param[in] sk_psd The public seed.
 * @param[in] target_height The height that the Treehash instance that should be
 * updated operates on.
 * @param[out] state The BDS/MMT state containing the Treehash instance & shared
 * stack.
 * @param[in] layer The hypertree layer that state operates on.
 */
void update_treehash(const unsigned char sk_psd[PYR_N],
                     const uint32_t target_height, state_type *state,
                     const uint32_t layer) {
  uint32_t addr[8] = {0};
  unsigned char node[2 * PYR_N];
  unsigned char thash_h_in[2 * PYR_N];
  uint64_t idx_tree;
  uint32_t idx_leaf;
  uint32_t j;

  /* Tree/leaf for the right node that we start combining from. */
  idx_tree = (state->treehash + target_height)->idx >> PYR_TREE_HEIGHT;
  idx_leaf = (uint32_t)((state->treehash + target_height)->idx &
                        ((1ULL << PYR_TREE_HEIGHT) - 1ULL));

  set_layer_addr(addr, layer);
  set_tree_addr(addr, idx_tree);
  set_type(addr, PYR_ADDR_TYPE_HASHTREE);
  set_tree_index(addr, idx_leaf);

  /* Create the idx_leaf-th node that we combine for. */
  tree_hash(node, (state->treehash + target_height)->ref_active,
            (state->treehash + target_height)->ref_active, sk_psd, idx_leaf, 0,
            addr);

  /* Combine whilst we have nodes of the same height > 0,
     and we do not exceed the target height (important for scheduling). */
  j = 0;
  while (state->stack->size > 0 &&
         state->stack->nodeheight[state->stack->size - 1] == j &&
         j < target_height) {
    set_tree_index(addr, idx_leaf >> (j + 1));
    set_tree_height(addr, j);
    stack_pop(state->stack, thash_h_in);
    memcpy(thash_h_in + PYR_N, node, PYR_N);
    thash(node, thash_h_in, 2, sk_psd, addr);
    j++;
  }
  /* The node does not reach the target height;
     push it onto the stack and increase idx by 1. */
  if (j < target_height) {
    stack_push(state->stack, node, j);
    (state->treehash + target_height)->idx++;
  } else { /* The node reaches the target height; treehash instance is
              completed. */
    (state->treehash + target_height)->fin = 1;
    (state->treehash + target_height)->idx = 0;
    if (layer == 0) {
      memcpy((state->treehash + target_height)->node, node, PYR_N);
    } else {
      stack_push(state->stack, node, j);
    }
  }
}

/**
 * @brief Given XMSS signature, return the root of XMSS tree. See RFC8391.
 * @details Generates the WOTS pk from the WOTS signature using the message. The
 * public key is compressed and combined with authentication path nodes, until
 * the root is reached.
 * @param[out] root The root of the XMSS tree.
 * @param[in] idx_leaf The index of the OTS that generates the WOTS signature.
 * @param[in] sig An XMSS signature: WOTS signature and authentication path.
 * @param[in] msg The compressed message that is signed by the OTS.
 * @param[in] sk_psd The public seed.
 * @param[in] subtree_addr The address of the tree that the leaf that is signed
 * with is located in. [layer address] and [tree address] should be set.
 */
void xmss_root_from_sig(unsigned char root[PYR_N], const uint32_t idx_leaf,
                        const unsigned char sig[SIG_XMSS_BYTES],
                        const unsigned char msg[PYR_N],
                        const unsigned char sk_psd[PYR_N],
                        const uint32_t subtree_addr[8]) {
  uint32_t wots_addr[8] = {0};
  uint32_t wotspk_addr[8] = {0};
  unsigned char wots_pk[PYR_WOTS_PK_BYTES];
  unsigned char leaf[PYR_N];

  copy_subtree_addr(wots_addr, subtree_addr);
  copy_subtree_addr(wotspk_addr, subtree_addr);
  set_type(wots_addr, PYR_ADDR_TYPE_WOTS);
  set_type(wotspk_addr, PYR_ADDR_TYPE_WOTSPK);

  /* Compute leaf (compressed WOTS public key). */
  set_keypair_addr(wots_addr, idx_leaf);
  wots_pk_from_sig(wots_pk, sig, msg, sk_psd, wots_addr);
  set_keypair_addr(wotspk_addr, idx_leaf);
  thash(leaf, wots_pk, PYR_WOTS_LEN, sk_psd, wotspk_addr);

  /* Traversal from leaf using the authentication path in signature. */
  compute_root(root, leaf, idx_leaf, sig + PYR_WOTS_BYTES, PYR_TREE_HEIGHT,
               sk_psd, subtree_addr);
}

/**
 * @brief Forwards the FSG reference nr_apply times.
 * @details The address should be in line with the expected input of fsg:
 * the tree layer, tree address and the keypair address should be set.
 * @param[out] next_ref The forwarded FSG reference.
 * @param[in] fsg_ref The FSG reference to start the forwarding from.
 * @param[in] pub_seed The public seed.
 * @param[in] nr_apply The number of times to apply FSG.
 * @param[in] start_addr The address to start forwarding from. The address
 * should correspond to fsg_ref. [layer address], [tree address] and [key pair
 * address] should be set.
 */
void fwd_ref(unsigned char *next_ref, const unsigned char *fsg_ref,
             const unsigned char *pub_seed, uint32_t nr_apply,
             const uint32_t start_addr[8]) {
  uint32_t i;
  uint32_t iter_addr[8] = {0};

  copy_keypair_addr(iter_addr, start_addr);
  memcpy(next_ref, fsg_ref, PYR_N);
  for (i = 0; i < nr_apply; i++) {
    fsg(NULL, next_ref, next_ref, pub_seed, iter_addr);
    increment_address(iter_addr);
  }
}

/**
 * @brief Generate an BDS state from the core of a Pyramid secret key.
 * @details Given a secret key, initializes all fields within an MMT state.
 * @param[in] sk The Pyramid secret key core.
 * @param[out] state The BDS state to initialize.
 */
void bds_stateGen(const unsigned char sk[PYR_SK_BYTES], state_type *state) {
  const unsigned char *const sk_ssd = sk + SK_SSD;
  const unsigned char *const sk_psd = sk + SK_PSD;
  uint32_t addr[8] = {0};
  unsigned char thash_h_in[2 * PYR_N];
  unsigned char next_ref[PYR_N];
  unsigned int i;

  set_layer_addr(addr, 0);
  set_tree_addr(addr, 0);
  set_type(addr, PYR_ADDR_TYPE_HASHTREE);

  /* 2.1. auth[0] */
  set_keypair_addr(addr, 0);
  fsg(NULL, next_ref, sk_ssd, sk_psd, addr);
  tree_hash(state->auth, next_ref, next_ref, sk_psd, 1, 0, addr);

  /* 2.2. auth[1]...auth[h/d-1] and treehash[0]...treehash[h/d-1],
     order ensures next_ref is forwarded a minimal number of times.*/
  set_tree_index(addr, 1);
  for (i = 0; i < PYR_TREE_HEIGHT - 1; i++) {
    /* 2.2.2. auth[i] = H(left, right), generate the left node.. */
    tree_hash(thash_h_in, next_ref, next_ref, sk_psd, 2 * (1 << i), i, addr);

    /* treehash[i] reference follows from the previous tree_hash call */
    memcpy((state->treehash + i)->ref_next, next_ref, PYR_N);

    /* 2.2.1. treehash[i] using- and forwarding next_ref. */
    tree_hash((state->treehash + i)->node, next_ref, next_ref, sk_psd,
              3 * (1 << i), i, addr);
    (state->treehash + i)->fin = 1;
    (state->treehash + i)->idx = 0;

    /* auth[i+1] = H(left, right), copy over the right node and finish auth[i+1]. */
    memcpy(thash_h_in + PYR_N, (state->treehash + i)->node, PYR_N);
    set_tree_height(addr, i);
    thash(state->auth + (i + 1) * PYR_N, thash_h_in, 2, sk_psd, addr);
  } /* End at next_ref for (1 << PYR_TREE_HEIGHT). */

  /* 4. Initialization of highest instance of h/d - 1 in the next XMSS tree. */
  (state->treehash + PYR_TREE_HEIGHT - 1)->fin = 1;
  (state->treehash + PYR_TREE_HEIGHT - 1)->idx = 0;

  if (PYR_D > 1) {
    /* FS: Forward the reference to the right tree. */
    set_tree_addr(addr, 1);
    set_keypair_addr(addr, 0);
    fwd_ref(next_ref, next_ref, sk_psd, (1 << (PYR_TREE_HEIGHT - 1)), addr);
    memcpy((state->treehash + PYR_TREE_HEIGHT - 1)->ref_next, next_ref, PYR_N);

    /* FS: starting at the previous next_ref for (1 << PYR_TREE_HEIGHT). */
    set_tree_addr(addr, 1);

    /* Adapted the starting index to a bugfix (see github). */
    tree_hash((state->treehash + PYR_TREE_HEIGHT - 1)->node, NULL, next_ref,
              sk_psd, (1 << (PYR_TREE_HEIGHT - 1)), PYR_TREE_HEIGHT - 1, addr);
  } else {
    memset((state->treehash + PYR_TREE_HEIGHT - 1)->ref_next, 0, PYR_N);
    memset((state->treehash + PYR_TREE_HEIGHT - 1)->node, 0, PYR_N);
  }

  /* Init remaining entries. */
  state->stack->size = 0;
  memset(state->stack->node, 0, (PYR_TREE_HEIGHT - 1) * PYR_N);
  memset(state->stack->nodeheight, 0, PYR_TREE_HEIGHT - 1);
  memset(state->keep, 0, (PYR_TREE_HEIGHT / 2) * PYR_N);

  memset(state->treehash->ref_active, 0, PYR_TREE_HEIGHT * PYR_N);
}

/**
 * @brief Generate an MMT state from the core of a Pyramid secret key.
 * @details Given a secret key, initializes all fields within an MMT state.
 * @param[in] sk The Pyramid secret key.
 * @param[out] state The MMT state to initialize.
 * @param[in] layer The hypertree layer of the MMT state.
 */
void mmt_stateGen(const unsigned char sk[PYR_SK_BYTES], state_type *state,
                  uint32_t layer) {
  const unsigned char *sk_ssd = sk + SK_SSD + layer * PYR_N;
  const unsigned char *sk_psd = sk + SK_PSD;
  uint32_t addr[8] = {0};
  unsigned char next_ref[PYR_N];
  unsigned int i;

  set_layer_addr(addr, layer);
  set_tree_addr(addr, 0);

  /* FS: move towards the first keypair that is involved in the first
     authentication path. */
  set_keypair_addr(addr, 0);
  fsg(NULL, next_ref, sk_ssd, sk_psd, addr);

  /* 2. Generate authentication path */
  /* FS: Juggle the FSG reference between trees, using next_ref. */
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    set_keypair_addr(addr, 1 << i);
    tree_hash(state->auth + i * PYR_N, next_ref, next_ref, sk_psd, (1 << i), i,
              addr);
    (state->treehash + i)->fin = 1;
    (state->treehash + i)->idx = 0;

    /* FS: set the active reference (intuitively this is "iteration -1"). */
    memcpy((state->treehash + i)->ref_active, next_ref, PYR_N);
  }

  /* 3. Init treehash instance of 0. */
  state->treehash->fin = 0;
  /* Init other entries. */
  state->stack->size = 0;
  memset(state->stack->node, 0, PYR_TREE_HEIGHT * PYR_N);
  memset(state->stack->nodeheight, 0, PYR_TREE_HEIGHT);
}

/**
 * @brief Summary Forwards the FSG references in a BDS state.
 * @details Forward once on every tree layer for every BDS state update.
 */
inline void bds_fwd_refs(const unsigned char sk[PYR_SK_BYTES],
                         state_type *state) {
  uint32_t th_ref_addr[8] = {0};
  uint32_t i, th_ref_idx_leaf;
  uint64_t idx_sig, th_ref_idx, th_ref_idx_tree;

  idx_sig = bytes_to_ull(sk + SK_IDX, PYR_IDX_BYTES);

  /* Intuition: the references on each layer are "pre-forwarded" during key
     generation; new treehash instances use those forwarded references at a
     fixed, layer-dependent offset: d(i) = 3 * (1 << i).*/
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    th_ref_idx = 3 * (1 << i) + idx_sig;

    /* Extreme case: PYR_TREE_HEIGTH == IDX_BITS. */
#if 2 + IDX_BITS > 64
#error "64-bit overflow. "
#endif

    /* If the forwarded index is no longer valid, do not generate its tree. */
    if (!invalid_idxp(th_ref_idx)) {
      th_ref_idx_tree = th_ref_idx >> PYR_TREE_HEIGHT;
      th_ref_idx_leaf = th_ref_idx & ((1 << PYR_TREE_HEIGHT) - 1);
      set_keypair_addr(th_ref_addr, th_ref_idx_leaf);
      set_tree_addr(th_ref_addr, th_ref_idx_tree);

      fsg(NULL, (state->treehash + i)->ref_next,
          (state->treehash + i)->ref_next, sk + SK_PSD, th_ref_addr);
    }
  }
}

/**
 * @brief Update BDS state after signature generation.
 * @details 1. Forwards the FSG regerences kept in state;
 *          2. Schedules additional Treehash instances,
 *             update the authentication path;
 *          3. Performs Treehash updates.
 * @param[in] sk The Pyramid secret key.
 * @param[inout] state The BDS state to update.
 */
void bds_state_update(const unsigned char sk[PYR_SK_BYTES], state_type *state) {
  const unsigned char *const sk_idx = sk + SK_IDX;
  const unsigned char *const sk_ssd = sk + SK_SSD;
  const unsigned char *const sk_psd = sk + SK_PSD;
  uint32_t addr[8] = {0};
  unsigned char thash_h_in[2 * PYR_N];
  uint32_t i, j, idx_leaf, tau;
  uint64_t idx_tmp, idx_tree, idx_sig;

  /* Full index (because layer = 0), tree index, leaf index. */
  idx_sig = bytes_to_ull(sk_idx, PYR_IDX_BYTES);
  idx_tree = idx_sig >> PYR_TREE_HEIGHT;
  idx_leaf = idx_sig & ((1ULL << PYR_TREE_HEIGHT) - 1ULL);

  set_tree_addr(addr, idx_tree);

  bds_fwd_refs(sk, state);

  /* 2. Compute tau, i.e., height of the first left parent of leafnode. */
  tau = get_tau(idx_leaf);
  /* 3. Backup keep[(tau-1)>>1] */
  if (tau > 0) {
    memcpy(thash_h_in, state->auth + (tau - 1) * PYR_N, PYR_N);
    memcpy(thash_h_in + PYR_N, state->keep + ((tau - 1) >> 1) * PYR_N, PYR_N);
  }
  /* 4. Keep auth[tau] in keep. */
  if (!((idx_leaf >> (tau + 1)) & 1) && (tau < PYR_TREE_HEIGHT - 1)) {
    memcpy(state->keep + (tau >> 1) * PYR_N, state->auth + tau * PYR_N, PYR_N);
  }
  /* 5. if tau = 0, generate auth[0]. */
  if (tau == 0) {
    /* We omit one optimization here: one could generate auth[0] from the WOTS
       signature, for ~1/2 the calls to F. This is left out to preserve
       readability. */
    tree_hash(state->auth, NULL, sk_ssd, sk_psd, idx_leaf, 0, addr);
  }
  /* 6. if tau > 0, */
  else {
    /* 6.1. Compute a hash value of keep[(tau-1)>>1] and auth[tau-1] to get new
       auth[tau]. */
    if (tau < PYR_TREE_HEIGHT) {
      set_type(addr, PYR_ADDR_TYPE_HASHTREE);
      set_tree_height(addr, tau - 1);
      set_tree_index(addr, idx_leaf >> tau);
      thash(state->auth + tau * PYR_N, thash_h_in, 2, sk_psd, addr);
    }
    /* 6.2. Copy treehash.node to auth and initialize copied treehash instances
       from 0 to tau-1. */
    for (j = 0; j < tau; j++) {
      memcpy(state->auth + j * PYR_N, (state->treehash + j)->node, PYR_N);

      memset((state->treehash + j)->node, 0, PYR_N);
      idx_tmp = ((uint64_t)idx_sig + 1ULL + 3ULL * (1ULL << j));
      if (idx_tmp < (uint64_t)(1ULL << PYR_FULL_HEIGHT)) {
        (state->treehash + j)->idx = idx_tmp;
        (state->treehash + j)->fin = 0;

        /* FS: replace the active reference with the next (forwarded) reference.
         */
        memcpy((state->treehash + j)->ref_active,
               (state->treehash + j)->ref_next, PYR_N);
      }
    }
  }
  /* 7. Treehash updates. */
  for (i = 0; i < (PYR_TREE_HEIGHT + 1) / 2; i++) {
    /* 7.1. Get the lowest unfinished treehash instance. */
    j = 0;
    while (j < PYR_TREE_HEIGHT && (state->treehash + j)->fin == 1) {
      j++;
    }

    /* 7.2. Update treehash[j] and stack if needed. */
    if (j < PYR_TREE_HEIGHT) {
      update_treehash(sk_psd, j, state, 0);
    } else {
      i = (PYR_TREE_HEIGHT + 1) / 2;
    }
  }
}

/**
 * @brief Updates a ghost instance of Treehash.
 * @details Update a Treehash instance that computes a left node in regular MMT.
 * The Treehash instance is located at height target_height, computing the node
 * at tree offset idx_leaf.
 */
inline void update_ghost_treehash(state_type *state, uint32_t target_height,
                                  uint32_t idx_leaf) {
  uint32_t taui, tree_idx, i;
  unsigned char zero[PYR_N] = {0};

  tree_idx =
      (state->treehash + target_height)->idx & ((1 << PYR_TREE_HEIGHT) - 1);
  taui = get_tau(tree_idx);

  /* Simulate popping previous nodes for combining. */
  for (i = 0; i < taui; ++i) {
    stack_pop(state->stack, zero);
  }
  /* Simulate pushing the result. */
  /* Check if the result would have been auth path node */
  if (((tree_idx >> taui) ^ 1) == (idx_leaf >> taui)) {
    stack_push(state->stack, state->auth + taui * PYR_N, taui);
  } else {
    /* If we did not push a relevant node, fake one. */
    stack_push(state->stack, zero, taui);
  }

  (state->treehash + target_height)->idx++;
}

/**
 * @brief Forwards the FSG references in an MMT state.
 * @details It is important to note that this code guarded behind a conditional
 in mmt_state_update; it is called every time that we hop a leaf on this layer.
 */
inline void mmt_fwd_refs(const unsigned char sk[PYR_SK_BYTES],
                         state_type *state, uint32_t layer) {
  uint32_t th_ref_addr[8] = {0};
  uint64_t idx_sig, th_ref_idx, th_ref_idx_tree, max_layer_idx;
  uint32_t i, idx_leaf, th_ref_idx_leaf;

  idx_sig =
      bytes_to_ull(sk + SK_IDX, PYR_IDX_BYTES) >> (layer * PYR_TREE_HEIGHT);
  idx_leaf = idx_sig & ((1ULL << PYR_TREE_HEIGHT) - 1ULL);
  max_layer_idx = 1ULL << (PYR_FULL_HEIGHT - (layer * PYR_TREE_HEIGHT));

  set_layer_addr(th_ref_addr, layer);
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    th_ref_idx = 1 + (1 << i) + idx_sig;

    /* idx_sig loses j bits from IDX_BITS at layer j */
#if 1 + IDX_BITS > 64
#error "64-bit overflow possible."
#endif

    /* mmt_fwd_refs is called when we hop a leaf node. Thus, idx_leaf enumerates
       hops in the current tree. When the ith bit is set, the ith reference is
       forwarded; it remains frozen otherwise. */
    if ((((idx_leaf + 1) >> i) & 1) == 1 && th_ref_idx < max_layer_idx) {
      th_ref_idx_tree = th_ref_idx >> PYR_TREE_HEIGHT;
      th_ref_idx_leaf = th_ref_idx & ((1 << PYR_TREE_HEIGHT) - 1);
      set_tree_addr(th_ref_addr, th_ref_idx_tree);
      set_keypair_addr(th_ref_addr, th_ref_idx_leaf);

      fsg(NULL, (state->treehash + i)->ref_active,
          (state->treehash + i)->ref_active, sk + SK_PSD, th_ref_addr);
    }
  }
}

/**
 * @brief Update the MMT state after signature generation.
 * @details 1. Forwards the FSG regerences kept in state;
 *          2. Schedules additional Treehash instances,
 *             update the authentication path.
 *          3. Performs one Treehash update.
 * @param[in] sk The Pyramid secret key.
 * @param[inout] state The MMT state to update.
 * @param[in] layer The hypertree layer of the MMT state.
 */
void mmt_state_update(const unsigned char sk[PYR_SK_BYTES], state_type *state,
                      uint32_t layer) {
  const unsigned char *sk_idx = sk + SK_IDX;
  const unsigned char *sk_ssd = sk + SK_SSD + layer * PYR_N;
  const unsigned char *sk_psd = sk + SK_PSD;
  unsigned char tmp[PYR_N];
  uint64_t idx_next, idx_sig, max_layer_idx;
  uint32_t i, j, idx_leaf, tau1, tau2;

  /* For checking whether an index is valid in the layer. */
  max_layer_idx = 1ULL << (PYR_FULL_HEIGHT - (layer * PYR_TREE_HEIGHT));

  /* The layer index, full index of the next node, leaf index. */
  idx_sig = bytes_to_ull(sk_idx, PYR_IDX_BYTES);
  idx_next = idx_sig + 1ULL;
  idx_sig = idx_sig >> (layer * PYR_TREE_HEIGHT);
  idx_leaf = idx_sig & ((1ULL << PYR_TREE_HEIGHT) - 1ULL);

  /* 2. Update authentication path and initialize treehash instances. */
  if ((idx_next & ((1ULL << (layer * PYR_TREE_HEIGHT)) - 1ULL)) == 0) {
    mmt_fwd_refs(sk, state, layer);

    tau1 = get_tau(idx_leaf);
    for (i = 0; i < (tau1 + 1) && i < PYR_TREE_HEIGHT; i++) {
      stack_pop(state->stack, state->auth + i * PYR_N);
    }
    tau2 = get_tau(idx_leaf + 1);
    for (i = 0; i < (tau2 + 1) && i < PYR_TREE_HEIGHT; i++) {
      if (i == tau2) {
        (state->treehash + i)->idx = idx_sig + 2ULL - (1ULL << i);
        (state->treehash + i)->fin = 0;
      } else if ((idx_sig + 2ULL + (1ULL << i)) < max_layer_idx) {
        (state->treehash + i)->idx = (uint64_t)(idx_sig + 2ULL + (1ULL << i));
        (state->treehash + i)->fin = 0;
      }
    }

    /* 3. Treehash update (only once). */
  } else if ((idx_next & ((1ULL << ((layer - 1) * PYR_TREE_HEIGHT)) - 1ULL)) ==
             0) {
    /* 3.1. Get the highest unfinished treehash instance. */
    j = PYR_TREE_HEIGHT;
    while (j > 0 && (state->treehash + j - 1)->fin == 1) {
      j--;
    }

    /* 3.2. Update treehash[j] and stack if needed. */
    if (j > 0) {
      i = j - 1;

      /* FS: Three cases:
         1. The target node is a left node: simulate the Treehash instance.
         2. The target node is the current node, which will complete
         the ghost Treehash instance. Swap references so ref_active remains at
         a constant distance. Current nodes can use sk_ssd.
         3. The target node is a right node: generate it normally. */
      if ((state->treehash + i)->idx < idx_sig) {
        update_ghost_treehash(state, i, idx_leaf);

      } else if ((state->treehash + i)->idx == idx_sig) {
        /* We omit one optimization here: one could generate auth[0] from the
           WOTS signature, for ~1/2 the calls to F. This is left out to preserve
           readability. */
        memcpy(tmp, (state->treehash + i)->ref_active, PYR_N);
        memcpy((state->treehash + i)->ref_active, sk_ssd, PYR_N);
        update_treehash(sk_psd, i, state, layer);
        memcpy((state->treehash + i)->ref_active, tmp, PYR_N);

      } else {
        update_treehash(sk_psd, i, state, layer);
      }
    }
  }
}

/**
 * @brief Serialize the state into the secret key.
 * @details Stores the non-pointers from state back into sk, i.e. byte sequence.
 * @param[inout] sk The sequence of bytes representing a secret key.
 * @param[in] state The state to copy members from.
 * @param[in] layer The hypertree layer of the state that is copied from.
 */
void state_serialize(unsigned char sk[PYR_SK_BYTES], const state_type *state,
                     unsigned int layer) {
  unsigned int i;

  /* Authentication path */
  sk += PYR_TREE_HEIGHT * PYR_N;
  /* Stack */
  sk[0] = state->stack->size;
  sk++;
  if (layer == 0) {
    sk += (PYR_TREE_HEIGHT - 1) * (PYR_N + 1);
  } else {
    sk += PYR_TREE_HEIGHT * (PYR_N + 1);
  }
  /* Treehash */
  if (layer == 0) {
    sk += PYR_TREE_HEIGHT * PYR_N;
  }
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    sk[0] = (state->treehash + i)->fin;
    sk += 1;
  }
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    ull_to_bytes(sk, PYR_IDX_BYTES, (state->treehash + i)->idx);
    sk += PYR_IDX_BYTES;
  }
  if (layer == 0) { /* FS BDS */
    for (i = 0; i < PYR_TREE_HEIGHT; i++) {
      sk += PYR_N; /* ref_active */
    }
    for (i = 0; i < PYR_TREE_HEIGHT; i++) {
      sk += PYR_N; /* ref_next */
    }
  } else { /* FS MMT */
    for (i = 0; i < PYR_TREE_HEIGHT; i++) {
      sk += PYR_N; /* ref_active */
    }
  }
}

/**
 * @brief Deserializes the secret key into state.
 * @details Points each part of state to the appropriate part of sk. sk is
 * stored in a columnar fashion. Non-pointers are copied, not altered directly.
 * @param[in] sk The sequence of bytes representing a secret key.
 * @param[out] state The state to initialize.
 * @param[in] layer The hypertree layer of the state that is initialized.
 */
void state_deserialize(const unsigned char *sk, state_type *state,
                       unsigned int layer) {
  unsigned int i;

  /* Authentication path */
  state->auth = (unsigned char *)sk;
  sk += PYR_TREE_HEIGHT * PYR_N;
  /* Stack */
  state->stack->size = sk[0];
  sk++;
  if (layer == 0) {
    state->stack->node = (unsigned char *)sk;
    sk += (PYR_TREE_HEIGHT - 1) * PYR_N;
    state->stack->nodeheight = (unsigned char *)sk;
    sk += PYR_TREE_HEIGHT - 1;
  } else {
    state->stack->node = (unsigned char *)sk;
    sk += PYR_TREE_HEIGHT * PYR_N;
    state->stack->nodeheight = (unsigned char *)sk;
    sk += PYR_TREE_HEIGHT;
  }
  /* Treehash */
  if (layer == 0) {
    for (i = 0; i < PYR_TREE_HEIGHT; i++) {
      (state->treehash + i)->node = (unsigned char *)sk;
      sk += PYR_N;
    }
  }
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    (state->treehash + i)->fin = sk[0];
    sk += 1;
  }
  for (i = 0; i < PYR_TREE_HEIGHT; i++) {
    (state->treehash + i)->idx = bytes_to_ull(sk, PYR_IDX_BYTES);
    sk += PYR_IDX_BYTES;
  }

  if (layer == 0) { /* FS BDS */
    for (i = 0; i < PYR_TREE_HEIGHT; i++) {
      (state->treehash + i)->ref_active = (unsigned char *)sk;
      sk += PYR_N;
    }
    for (i = 0; i < PYR_TREE_HEIGHT; i++) {
      (state->treehash + i)->ref_next = (unsigned char *)sk;
      sk += PYR_N;
    }
  } else { /* FS MMT */
    for (i = 0; i < PYR_TREE_HEIGHT; i++) {
      (state->treehash + i)->ref_active = (unsigned char *)sk;
      sk += PYR_N;
    }
  }

  if (layer == 0) {
    state->keep = (unsigned char *)sk;
  }
}

/**
 * @brief Return a pointer to the authentication path in sk on hypertree layer
 * layer.
 * @details Adds the core offset and layer-specific offsets to the pointer sk.
 * @param[in] sk The secret key that contains the authentication path.
 * @param[in] layer The hypertree layer to obtain the authentication path
 * pointer for.
 * @return Pointer to the authentication path on layer layer within sk.
 */
const unsigned char *sk_get_auth(const unsigned char sk[PYR_SK_BYTES],
                                 uint32_t layer) {
  const unsigned char *auth_path_layer = sk;

  auth_path_layer += PYR_SK_CORE_BYTES;
  if (layer != 0) {
    auth_path_layer += BDS_BYTES;
    auth_path_layer += (layer - 1) * MMT_BYTES;
  }
  return auth_path_layer;
}

/**
 * @brief Finalizes the secret key.
 * @details Initializes the simple state and computes the root of the top tree.
 * @param[out] root The root of the top XMSS tree.
 * @param[out] sk The partially initialized secret key.
 * @return 0: computation as expected.
 */
int xmssmt_finalize_sk(unsigned char root[PYR_N],
                       unsigned char sk[PYR_SK_BYTES]) {
  const unsigned char *const sk_ssd = sk + SK_SSD + (NR_SSD - 1) * PYR_N;
  const unsigned char *const sk_psd = sk + SK_PSD;
  unsigned char *sk_simple = sk + PYR_SK_CORE_BYTES;
  uint32_t addr[8] = {0};
  unsigned char leaf[PYR_N];
  state_type state;
  stack_type stack[1];
  treehash_type treehash[32];
  unsigned int i;

  /* 1.4. Memory allocation of members of state. */
  state.stack = stack;
  state.treehash = treehash;
  /* 2. Generate state. */
  /* 2.1. Generate BDS state. */
  state_deserialize(sk_simple, &state, 0);
  bds_stateGen(sk, &state);
  state_serialize(sk_simple, &state, 0);
  sk_simple += BDS_BYTES;
  /* 2.2. Generate MMT states. */
  for (i = 1; i < PYR_D; i++) {
    state_deserialize(sk_simple, &state, i);
    mmt_stateGen(sk, &state, i);
    state_serialize(sk_simple, &state, i);
    sk_simple += MMT_BYTES;
  }
  /* 3. Generate root of top XMSS tree. */
  /* FS: this part is altered to reuse the functionality of compute_root. */
  set_layer_addr(addr, PYR_D - 1);
  set_tree_addr(addr, 0);
  set_keypair_addr(addr, 0);
  tree_hash(leaf, NULL, sk_ssd, sk_psd, 0, 0, addr);
  compute_root(root, leaf, 0, state.auth, PYR_TREE_HEIGHT, sk_psd, addr);

  return 0;
}

/**
 * @brief Finalizes a signature with a HyperTree signature.
 * @details Puts the authentication path in the signature,
 *          followed by a WOTS signature, for every hypertree layer.
 * @param[in] sk The secret key to sign with.
 * @param[out] sig Pointer to the (full) signature.
 * @param[out] sig_len The length of sig (initially the HT signature offset).
 * @param[in] mhash The hashed message.
 * @return 0: computation as expected.
 */
int xmssmt_ht_sign(const unsigned char sk[PYR_SK_BYTES],
                   unsigned char sig[PYR_BYTES], size_t sig_len[1],
                   const unsigned char mhash[PYR_N]) {
  const unsigned char *const sk_ssd = sk + SK_SSD;
  const unsigned char *const sk_psd = sk + SK_PSD;
  const unsigned char *auth_path;
  uint32_t addr[8] = {0};
  unsigned char R[PYR_N];
  unsigned char root[PYR_N];
  unsigned int i;
  uint64_t idx_tree;
  uint32_t idx_leaf;

  /* Point to the HyperTree signature. */
  sig += SIG_HTS;

  /* Initialized to the leaf that we sign for. */
  idx_tree = bytes_to_ull(sk, PYR_IDX_BYTES);

  /* XMSS signatures for every layer. */
  for (i = 0; i < PYR_D; i++) {
    if (i == 0) { /* Intent: sign the message hash. */
      memcpy(root, mhash, PYR_N);
    } else { /* Intent: sign root on layer i - 1 */
      xmss_root_from_sig(root, idx_leaf, sig - SIG_XMSS_BYTES, root, sk_psd,
                         addr);
    }

    /* Previous layer done, update indices. */
    idx_leaf = (idx_tree & ((1 << PYR_TREE_HEIGHT) - 1));
    idx_tree >>= PYR_TREE_HEIGHT;

    set_layer_addr(addr, i);
    set_tree_addr(addr, idx_tree);
    set_keypair_addr(addr, idx_leaf);

    /* Sign root. */
    /* FS: generate the wots seed corresponding to the layer sk_ssd. */
    fsg(R, NULL, sk_ssd + i * PYR_N, sk_psd, addr);
    auth_path = sk_get_auth(sk, i);
    tree_sig(sig, root, R, sk_psd, idx_leaf, addr, auth_path);

    sig += SIG_XMSS_BYTES;
    *sig_len += SIG_XMSS_BYTES;
  }

  return 0;
}

/**
 * @brief Perform a state update for every BDS/MMT state in sk.
 * @details Given a secret key that was used for a Pyramid signature, update the
 * state at sk_simple. The state is kept in the secret key.
 * @param[inout] sk The Pyramid secret key.
 * @return 0: computation as expected.
 */
int xmssmt_update_state(unsigned char sk[PYR_SK_BYTES]) {
  unsigned char *sk_simple = sk + PYR_SK_CORE_BYTES;
  state_type state;
  stack_type stack[1];
  treehash_type treehash[32];
  unsigned int i;

  state.stack = stack;
  state.treehash = treehash;

  /* 1 BDS state update. */
  state_deserialize(sk_simple, &state, 0);
  bds_state_update(sk, &state);
  state_serialize(sk_simple, &state, 0);
  sk_simple += BDS_BYTES;

  /* 0+ MMT state update(s).*/
  for (i = 1; i < PYR_D; i++) {
    state_deserialize(sk_simple, &state, i);
    mmt_state_update(sk, &state, i);
    state_serialize(sk_simple, &state, i);
    sk_simple += MMT_BYTES;
  }

  return 0;
}

/* Key finalization of XMSS. */
int xmss_finalize_sk(unsigned char root[PYR_N],
                     unsigned char sk[PYR_SK_BYTES]) {
  return xmssmt_finalize_sk(root, sk);
}

/* Signature generation of XMSS. */
int xmss_t_sign(const unsigned char sk[PYR_SK_BYTES],
                unsigned char sig[PYR_BYTES], size_t sig_len[1],
                const unsigned char mhash[PYR_N]) {
  return xmssmt_ht_sign(sk, sig, sig_len, mhash);
}

/* State update of XMSS. */
int xmss_update_state(unsigned char sk[PYR_SK_BYTES]) {
  return xmssmt_update_state(sk);
}

#include "treehash.h"

#include <string.h>

#include "address.h"
#include "hash.h"
#include "params.h"
#include "thash.h"
#include "wots.h"

/**
 * Modified treehash algorithm that restores the stack.
 * For a given leaf index, computes the authentication path and the resulting
 * root node using Merkle's TreeHash algorithm.
 * Expects the layer and tree parts of subtree_addr to be set.
 *
 * FS: When c = 0, the caller does NOT request the next Treehash state.
 */
void treehash(unsigned char *root, unsigned char *auth_path,
              unsigned char *next_seed, unsigned char *th_stack, unsigned int c,
              const unsigned char *sk_seed, const unsigned char *pub_seed,
              uint32_t leaf_idx, const uint32_t subtree_addr[8]) {
  unsigned char stack[(PYR_TREE_HEIGHT + 1) * PYR_N];
  unsigned int heights[PYR_TREE_HEIGHT + 1];
  unsigned int offset = 0;

  /* The subtree has at most 2^24 leafs, so uint32_t suffices. */
  uint32_t idx;
  uint32_t tree_idx;

  /* We need all three types of addresses in parallel. */
  uint32_t wots_addr[8] = {0};
  uint32_t wotspk_addr[8] = {0};
  uint32_t hashtree_addr[8] = {0};

  unsigned char S[PYR_N]; /* WOTS Seed */
  unsigned char R[PYR_N]; /* FSG Reference */
  memcpy(R, sk_seed, PYR_N);

  /* FS: Restore all left nodes from the previous treehash stack in auth_path.
   */
  uint32_t i;
  for (i = PYR_TREE_HEIGHT - 1; i != UINT32_MAX; i--) {
    if ((leaf_idx >> i) & 1) {
      memcpy(&auth_path[i * PYR_N], &th_stack[offset * PYR_N], PYR_N);
      heights[offset] = i;
      offset++;
    }
  }

  /* Restore the entire previous Treehash stack. */
  memcpy(stack, th_stack, offset * PYR_N);

  /* Select the required subtree. */
  copy_subtree_addr(wots_addr, subtree_addr);
  copy_subtree_addr(wotspk_addr, subtree_addr);
  copy_subtree_addr(hashtree_addr, subtree_addr);

  set_type(wots_addr, PYR_ADDR_TYPE_WOTS);
  set_type(wotspk_addr, PYR_ADDR_TYPE_WOTSPK);
  set_type(hashtree_addr, PYR_ADDR_TYPE_HASHTREE);

  /* FS: Continue Treehash from the previous state. */
  for (idx = leaf_idx; idx < (uint32_t)(1 << PYR_TREE_HEIGHT); idx++) {
    /* Add the next leaf node to the stack. */
    set_keypair_addr(wotspk_addr, idx);
    set_keypair_addr(wots_addr, idx);

    fsg(S, R, R, pub_seed, wots_addr);
    gen_leaf_wots(stack + offset * PYR_N, S, pub_seed, wotspk_addr, wots_addr);
    offset++;
    heights[offset - 1] = 0;

    /* If this is a node we need for the auth path.. */
    if ((leaf_idx ^ 0x1) == idx) {
      memcpy(auth_path, stack + (offset - 1) * PYR_N, PYR_N);
    }

    /* While the top-most nodes are of equal height.. */
    while (offset >= 2 && heights[offset - 1] == heights[offset - 2]) {
      /* Compute index of the new node, in the next layer. */
      tree_idx = (idx >> (heights[offset - 1] + 1));

      /* Hash the top-most nodes from the stack together. */
      /* Note that tree height is the 'lower' layer, even though we use
         the index of the new node on the 'higher' layer. This follows
         from the fact that we address the hash function calls. */
      set_tree_height(hashtree_addr, heights[offset - 1]);
      set_tree_index(hashtree_addr, tree_idx);
      thash(stack + (offset - 2) * PYR_N, stack + (offset - 2) * PYR_N, 2,
            pub_seed, hashtree_addr);

      offset--;
      /* Note that the top-most node is now one layer higher. */
      heights[offset - 1]++;

      /* If this is a node we need for the auth path. */
      if (((leaf_idx >> heights[offset - 1]) ^ 0x1) == tree_idx) {
        memcpy(auth_path + heights[offset - 1] * PYR_N,
               stack + (offset - 1) * PYR_N, PYR_N);
      }
    }
    /* FS: Store a copy of the Treehash stack after one iteration, IF requested.
     */
    if (idx == leaf_idx && c == 1) {
      memcpy(&th_stack[(offset - 1) * PYR_N], &stack[(offset - 1) * PYR_N],
             PYR_N);
      memcpy(next_seed, R, PYR_N);
    }
  }

  memcpy(root, stack, PYR_N);
}

#include "merkle.h"

#include <string.h>

#include "address.h"
#include "convert.h"
#include "params.h"
#include "thash.h"

/**
 * Computes a root node given a leaf and an auth path
 */
void compute_root(unsigned char *root, const unsigned char *leaf,
                  unsigned long idx_leaf, const unsigned char *auth_path,
                  uint32_t tree_height, const unsigned char *pub_seed,
                  const uint32_t subtree_addr[8]) {
  uint32_t i;
  unsigned char buffer[2 * PYR_N];
  uint32_t hashtree_addr[8] = {0};

  copy_subtree_addr(hashtree_addr, subtree_addr);
  set_type(hashtree_addr, PYR_ADDR_TYPE_HASHTREE);
  set_tree_index(hashtree_addr, idx_leaf);

  /* If idx_leaf is odd (last bit = 1), current path element is a right child
     and auth_path has to go left. Otherwise it is the other way around. */
  if (idx_leaf & 1) {
    memcpy(buffer + PYR_N, leaf, PYR_N);
    memcpy(buffer, auth_path, PYR_N);
  } else {
    memcpy(buffer, leaf, PYR_N);
    memcpy(buffer + PYR_N, auth_path, PYR_N);
  }
  auth_path += PYR_N;

  for (i = 0; i < tree_height - 1; i++) {
    set_tree_height(hashtree_addr, i);
    idx_leaf >>= 1;
    set_tree_index(hashtree_addr, idx_leaf);

    /* Pick the right or left neighbor, depending on parity of the node. */
    if (idx_leaf & 1) {
      thash(buffer + PYR_N, buffer, 2, pub_seed, hashtree_addr);
      memcpy(buffer, auth_path, PYR_N);
    } else {
      thash(buffer, buffer, 2, pub_seed, hashtree_addr);
      memcpy(buffer + PYR_N, auth_path, PYR_N);
    }
    auth_path += PYR_N;
  }

  /* The last iteration is exceptional; we do not copy an auth_path node. */
  set_tree_height(hashtree_addr, tree_height - 1);
  idx_leaf >>= 1;
  set_tree_index(hashtree_addr, idx_leaf);
  thash(root, buffer, 2, pub_seed, hashtree_addr);
}

#ifndef PYR_MERKLE_H
#define PYR_MERKLE_H

#include <stdint.h>

/**
 * For a given leaf index, computes the authentication path and the resulting
 * root node using Merkle's TreeHash algorithm.
 * Expects the layer and tree parts of subtree_addr to be set.
 */
void compute_root(unsigned char *root, const unsigned char *leaf,
                  unsigned long idx_leaf, const unsigned char *auth_path,
                  uint32_t tree_height, const unsigned char *pub_seed,
                  const uint32_t subtree_addr[8]);
		  
/*
  Jasmin testing export function definition.
*/
void compute_root_jazz(unsigned char *root, const unsigned char *leaf,
                       const unsigned char *auth_path,
                       const unsigned char *pub_seed,
                       const uint32_t subtree_addr[8],
                       const uint32_t other[2]);

#endif

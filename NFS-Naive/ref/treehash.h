#ifndef PYR_TREEHASH_H
#define PYR_TREEHASH_H

#include <stdint.h>

#include "params.h"

/**
 * For a given leaf index, computes the authentication path and the resulting
 * root node using Merkle's TreeHash algorithm.
 * Expects the layer and tree parts of subtree_addr to be set.
 */
void treehash(unsigned char *root, unsigned char *auth_path,
              const unsigned char *sk_seed, const unsigned char *pub_seed,
              uint32_t leaf_idx, const uint32_t subtree_addr[8]);

#endif

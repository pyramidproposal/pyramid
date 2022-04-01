#include "address.h"

#include <stdint.h>
#include <string.h>

#include "convert.h"
#include "params.h"

#define PYR_OFFSET_DIRECTION PYR_OFFSET_CHAIN_ADDR

/*
 * Specify which level of Merkle tree (the "layer") we're working on.
 */
void set_layer_addr(uint32_t addr[8], uint32_t layer) {
  ((unsigned char *)addr)[PYR_OFFSET_LAYER] = layer;
}

/*
 * Specify which Merkle tree within the level (the "tree address") we're working
 * on.
 */
void set_tree_addr(uint32_t addr[8], uint64_t tree) {
#if (PYR_TREE_HEIGHT * (PYR_D - 1)) > 64
#error Subtree addressing is currently limited to at most 2^64 trees
#endif
  ull_to_bytes(&((unsigned char *)addr)[PYR_OFFSET_TREE], 8, tree);
}

/*
 * Specify the reason we'll use this address structure for, that is, what
 * hash will we compute with it.  This is used so that unrelated types of
 * hashes don't accidentally get the same address structure.  The type will be
 * one of the PYR_ADDR_TYPE constants
 */
void set_type(uint32_t addr[8], uint32_t type) {
  ((unsigned char *)addr)[PYR_OFFSET_TYPE] = type;
}

/*
 * Copy the layer and tree fields of the address structure.  This is used
 * when we're doing multiple types of hashes within the same Merkle tree
 */
void copy_subtree_addr(uint32_t out[8], const uint32_t in[8]) {
  ((unsigned char *)out)[PYR_OFFSET_LAYER] =
      ((unsigned char *)in)[PYR_OFFSET_LAYER];
  memcpy((unsigned char *)out + PYR_OFFSET_TREE,
         (unsigned char *)in + PYR_OFFSET_TREE, 8);
}

/* These functions are used for OTS addresses. */

/*
 * Specify which Merkle leaf we're working on; that is, which OTS keypair
 * we're talking about.
 */
void set_keypair_addr(uint32_t addr[8], uint32_t keypair) {
  /* Because SPX explicitly defines multiple offsets instead of one,
     we do the same to be safe. Otherwise we would ull_to_bytes for clarity. */
#if PYR_TREE_HEIGHT > 16
  ((unsigned char *)addr)[PYR_OFFSET_KP_ADDR3] = keypair >> 16;
#endif
#if PYR_TREE_HEIGHT > 8
  ((unsigned char *)addr)[PYR_OFFSET_KP_ADDR2] = keypair >> 8;
#endif
  ((unsigned char *)addr)[PYR_OFFSET_KP_ADDR1] = keypair;
}

/*
 * Copy the layer, tree and keypair fields of the address structure.  This is
 * used when we're doing multiple things within the same OTS keypair
 */
void copy_keypair_addr(uint32_t out[8], const uint32_t in[8]) {
  copy_subtree_addr(out, in);
#if PYR_TREE_HEIGHT > 16
  ((unsigned char *)out)[PYR_OFFSET_KP_ADDR3] =
      ((unsigned char *)in)[PYR_OFFSET_KP_ADDR3];
#endif
#if PYR_TREE_HEIGHT > 8
  ((unsigned char *)out)[PYR_OFFSET_KP_ADDR2] =
      ((unsigned char *)in)[PYR_OFFSET_KP_ADDR2];
#endif
  ((unsigned char *)out)[PYR_OFFSET_KP_ADDR1] =
      ((unsigned char *)in)[PYR_OFFSET_KP_ADDR1];
}

/*
 * Specify which Merkle chain within the OTS we're working with
 * (the chain address)
 */
void set_chain_addr(uint32_t addr[8], uint32_t chain) {
  ((unsigned char *)addr)[PYR_OFFSET_CHAIN_ADDR] = chain;
}

/*
 * Specify where in the Merkle chain we are
 * (the hash address)
 */
void set_hash_addr(uint32_t addr[8], uint32_t hash) {
  ((unsigned char *)addr)[PYR_OFFSET_HASH_ADDR] = hash;
}

/* These functions are used for all hash tree addresses. */

/*
 * Specify the height of the node in the Merkle tree we are in
 * (the tree height)
 */
void set_tree_height(uint32_t addr[8], uint32_t tree_height) {
  ((unsigned char *)addr)[PYR_OFFSET_TREE_HGT] = tree_height;
}

/*
 * Specify the distance from the left edge of the node in the Merkle tree
 * (the tree index).
 */
void set_tree_index(uint32_t addr[8], uint32_t tree_index) {
  u32_to_bytes(&((unsigned char *)addr)[PYR_OFFSET_TREE_INDEX], tree_index);
}

/*
 * Obtain the index of the Merkle tree within the level (the "tree address")
 * we're working on.
 */
uint64_t get_tree_addr(const uint32_t addr[8]) {
#if (PYR_TREE_HEIGHT * (PYR_D - 1)) > 64
#error Subtree addressing is currently limited to at most 2^64 trees
#endif
  return bytes_to_ull(&((unsigned char *)addr)[PYR_OFFSET_TREE], 8);
}

/*
 * Obtain the index of the Merkle leaf we're working on; that is, which OTS
 * keypair we're talking about.
 */
uint32_t get_keypair_addr(const uint32_t addr[8]) {
  uint32_t retval = 0;
#if PYR_TREE_HEIGHT > 16
  retval |= ((unsigned char *)addr)[PYR_OFFSET_KP_ADDR3] << 16;
#endif
#if PYR_TREE_HEIGHT > 8
  retval |= ((unsigned char *)addr)[PYR_OFFSET_KP_ADDR2] << 8;
#endif
  retval |= ((unsigned char *)addr)[PYR_OFFSET_KP_ADDR1];
  return retval;
}

/*
 * Obtain the height of the node in the Merkle tree we are in
 * (the tree height)
 */
uint32_t get_tree_height(const uint32_t addr[8]) {
  return ((unsigned char *)addr)[PYR_OFFSET_TREE_HGT];
}

/*
 * Obtain the distance from the left edge of the node in the Merkle tree
 * (the tree index).
 */
uint32_t get_tree_index(const uint32_t addr[8]) {
  return bytes_to_ull(&((unsigned char *)addr)[PYR_OFFSET_TREE_INDEX], 4);
}

/*
 * idx uniquely identifies a (keypair, tree) address at layer 0.
 */
void set_keypair_addr_bottom(uint32_t addr[8], uint64_t idx) {
  set_layer_addr(addr, 0);
  set_tree_addr(addr, idx >> PYR_TREE_HEIGHT);
  set_keypair_addr(addr, idx & ((1 << PYR_TREE_HEIGHT) - 1));
}

/*
 * Increment address, potentially to an address in a tree to its right.
 * Also safe when PYR_TREE_HEIGHT % 8 = 0 (allows carry).
 */
void increment_address(uint32_t addr[8]) {
  uint32_t c, keypair_addr;
  uint64_t tree_addr;

  keypair_addr = get_keypair_addr(addr);
  tree_addr = get_tree_addr(addr);
  keypair_addr++;
  c = (keypair_addr >> PYR_TREE_HEIGHT) & 1;
  keypair_addr &= (UINT32_MAX >> (32 - PYR_TREE_HEIGHT));
  set_keypair_addr(addr, keypair_addr);
  tree_addr += c;
  set_tree_addr(addr, tree_addr);
}

void set_direction(uint32_t addr[8], uint32_t direction) {
  ((unsigned char *)addr)[PYR_OFFSET_DIRECTION] = direction;
}

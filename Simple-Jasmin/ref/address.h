#ifndef PYR_ADDRESS_H
#define PYR_ADDRESS_H

#include <stdint.h>

/* The hash types that are passed to set_type */
#define PYR_ADDR_TYPE_WOTS 0
#define PYR_ADDR_TYPE_WOTSPK 1
#define PYR_ADDR_TYPE_HASHTREE 2

/* Pyramid additional types */
#define PYR_ADDR_TYPE_HMSG 3
#define PYR_ADDR_TYPE_PRF 4
#define PYR_ADDR_TYPE_PRFKG 5
#define PYR_ADDR_TYPE_FSG 6

void set_layer_addr(uint32_t addr[8], uint32_t layer);

void set_tree_addr(uint32_t addr[8], uint64_t tree);

void set_type(uint32_t addr[8], uint32_t type);

void copy_subtree_addr(uint32_t out[8], const uint32_t in[8]);

void set_keypair_addr(uint32_t addr[8], uint32_t keypair);

void set_chain_addr(uint32_t addr[8], uint32_t chain);

void set_hash_addr(uint32_t addr[8], uint32_t hash);

void copy_keypair_addr(uint32_t out[8], const uint32_t in[8]);

void set_tree_height(uint32_t addr[8], uint32_t tree_height);

void set_tree_index(uint32_t addr[8], uint32_t tree_index);

uint32_t get_tree_height(const uint32_t addr[8]);

uint32_t get_tree_index(const uint32_t addr[8]);

void set_keypair_addr_bottom(uint32_t addr[8], uint64_t idx);

void increment_address(uint32_t addr[8]);

void set_direction(uint32_t addr[8], uint32_t direction);

/*
   The following functions have their static modifier removed for tests.
*/
uint64_t get_tree_addr(const uint32_t addr[8]);

uint32_t get_keypair_addr(const uint32_t addr[8]);

/*
  Jasmin testing export function definitions.
*/
void set_layer_addr_jazz(uint32_t addr[8], uint32_t layer);

void set_tree_addr_jazz(uint32_t addr[8], uint64_t tree);

void set_type_0xff_jazz(uint32_t addr[8]);

void copy_subtree_addr_jazz(uint32_t out[8], const uint32_t in[8]);

void set_keypair_addr_jazz(uint32_t addr[8], uint32_t keypair);

void set_chain_addr_jazz(uint32_t addr[8], uint32_t chain);

void set_hash_addr_jazz(uint32_t addr[8], uint32_t hash);

void copy_keypair_addr_jazz(uint32_t out[8], const uint32_t in[8]);

void set_tree_height_jazz(uint32_t addr[8], uint32_t tree_height);

void set_tree_index_jazz(uint32_t addr[8], uint32_t tree_index);

uint32_t get_tree_height_jazz(const uint32_t addr[8]);

uint32_t get_tree_index_jazz(const uint32_t addr[8]);

void increment_address_jazz(uint32_t addr[8]);

void encode_wots_address_jazz(unsigned char *out, const uint32_t addr[8]);

void set_direction_jazz(uint32_t addr[8], uint32_t direction);

uint64_t get_tree_addr_jazz(const uint32_t addr[8]);

uint32_t get_keypair_addr_jazz(const uint32_t addr[8]);

#endif

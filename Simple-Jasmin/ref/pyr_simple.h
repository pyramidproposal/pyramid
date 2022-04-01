#ifndef PYR_SIMPLE_H
#define PYR_SIMPLE_H
#include <stddef.h>
#include <stdint.h>

/*
  This file is based on "xmss_simple: Simple and Memory-efficient XMSS^MT".
  Original code: https://github.com/HaruCrypto54/xmss_simple on Sep 15, 2021.
  Original code author:         Haruhisa Kosuge (HaruCrypto54).
  Co-author accompanying paper: Hidema Tanaka.
 */

/**
 * @def SIG_XMSS_BYTES
 * WOTS signature, Authentication path.
 */
#define SIG_XMSS_BYTES (PYR_WOTS_BYTES + (PYR_TREE_HEIGHT * PYR_N))

/**
 * @def BDS_BYTES
 * Authentication path, Stack, Treehash, Keep.
 */
#define BDS_BYTES                                                          \
  ((PYR_TREE_HEIGHT * PYR_N) + 1 + ((PYR_TREE_HEIGHT - 1) * (PYR_N + 1)) + \
   (PYR_TREE_HEIGHT * (PYR_N + 1 + PYR_IDX_BYTES + 2 * PYR_N)) +           \
   ((PYR_TREE_HEIGHT / 2) * PYR_N))

/**
 * @def MMT_BYTES
 * Authentication path, Stack, Treehash.
 */
#define MMT_BYTES                                                    \
  ((PYR_TREE_HEIGHT * PYR_N) + 1 + (PYR_TREE_HEIGHT * (PYR_N + 1)) + \
   (PYR_TREE_HEIGHT * (1 + PYR_IDX_BYTES + PYR_N)))

/**
 * @def XMSSMT_STATE_BYTES
 * 1 BDS state, PYR_D-1 MMT states.
 */
#define XMSSMT_STATE_BYTES (BDS_BYTES + (PYR_D - 1) * MMT_BYTES)

// Key finalization of XMSS^MT
int xmssmt_finalize_sk(unsigned char *root, unsigned char *sk);

// Signature generation of XMSS^MT.
int xmssmt_ht_sign(const unsigned char *sk, unsigned char *sig, size_t *sig_len,
                   const unsigned char *mhash);

// State update of XMSS^MT
int xmssmt_update_state(unsigned char *sk);

// Key finalization of XMSS.
int xmss_finalize_sk(unsigned char *root, unsigned char *sk);

// Signature generation of XMSS.
int xmss_t_sign(const unsigned char *sk, unsigned char *sig, size_t *sig_len,
                const unsigned char *mhash);

// State update of XMSS.
int xmss_update_state(unsigned char *sk);

/*
  Typedefs moved to header for tests.
*/
typedef struct {
  unsigned char *node;
  unsigned char fin;
  uint64_t idx;
  unsigned char *ref_active;
  unsigned char *ref_next;
} treehash_type;

typedef struct {
  unsigned char size;
  unsigned char *node;
  unsigned char *nodeheight;
} stack_type;

typedef struct {
  unsigned char *auth;
  stack_type *stack;
  treehash_type *treehash;
  unsigned char *keep;
} state_type;

/*
   The following functions have their static modifier removed for tests.
*/
uint32_t get_tau(const uint32_t idx_leaf);

void stack_pop(stack_type *stack, unsigned char *pop_node);

void stack_push(stack_type *stack, const unsigned char *push_node,
                unsigned char push_nodeheight);

void tree_hash(unsigned char *root, unsigned char *next_seed,
               const unsigned char *sk_ssd, const unsigned char *sk_psd,
               uint32_t s, uint32_t t, const uint32_t subtree_addr[8]);

void tree_sig(unsigned char *sig, const unsigned char *M,
              const unsigned char *sk_ssd, const unsigned char *sk_psd,
              const uint32_t idx_leaf, const uint32_t subtree_addr[8],
              const unsigned char *auth_path);

void update_treehash(const unsigned char *sk_psd, const uint32_t target_height,
                     state_type *state, const uint32_t layer);

void update_treehash_debug(const unsigned char *sk_psd,
                           const uint32_t target_height, state_type *state,
                           const uint32_t layer);

void xmss_root_from_sig(unsigned char *root, const uint32_t idx_leaf,
                        const unsigned char *sig, const unsigned char *msg,
                        const unsigned char *sk_psd,
                        const uint32_t subtree_addr[8]);

void fwd_ref(unsigned char *next_ref, const unsigned char *fsg_ref,
             const unsigned char *pub_seed, uint32_t nr_apply,
             const uint32_t start_addr[8]);

void bds_stateGen(const unsigned char *sk, state_type *state);

void mmt_stateGen(const unsigned char *sk, state_type *state, uint32_t layer);

void bds_state_update(const unsigned char *sk, state_type *state);

void bds_state_update_debug(const unsigned char *sk, state_type *state);

void mmt_state_update(const unsigned char *sk, state_type *state,
                      uint32_t layer);

void mmt_state_update_debug(const unsigned char *sk, state_type *state,
                            uint32_t layer);

void state_serialize(unsigned char *sk, const state_type *state,
                     unsigned int layer);

void state_deserialize(const unsigned char *sk, state_type *state,
                       unsigned int layer);

const unsigned char *sk_get_auth(const unsigned char *sk, unsigned int layer);

int xmssmt_finalize_sk(unsigned char *root, unsigned char *sk);

int xmssmt_ht_sign(const unsigned char *sk, unsigned char *sig, size_t *sig_len,
                   const unsigned char *mhash);

int xmssmt_update_state(unsigned char *sk);

int xmss_finalize_sk(unsigned char *root, unsigned char *sk);

int xmss_t_sign(const unsigned char *sk, unsigned char *sig, size_t *sig_len,
                const unsigned char *mhash);

int xmss_update_state(unsigned char *sk);

/*
  Jasmin testing export function definitions.
*/
void treehash_jazz(unsigned char *root, unsigned char *next_seed,
                   const unsigned char *sk_ssd, const unsigned char *sk_psd,
                   const uint32_t subtree_addr[8], const uint32_t other[2]);

void tree_sig_jazz(unsigned char *sig, const unsigned char *M,
                   const unsigned char *sk, uint32_t idx_leaf,
                   const uint32_t subtree_addr[8],
                   const unsigned char *auth_path);

void BDS_update_treehash_jazz(const unsigned char *sk_psd,
                              const uint32_t target_height,
                              unsigned char *statep);

void MMT_update_treehash_jazz(const unsigned char *sk_psd,
                              const uint32_t target_height,
                              unsigned char *statep, const uint32_t layer);

void fwd_ref_jazz(unsigned char *next_ref, const unsigned char *pub_seed,
                  const uint32_t start_addr[8]);

void BDS_stategen_jazz(unsigned char *statep, const unsigned char *skp);

void MMT_stategen_jazz(unsigned char *statep, const unsigned char *skp,
                       uint32_t layer);

void BDS_state_update_jazz(unsigned char *statep, const unsigned char *skp);

void MMT_state_update_jazz(unsigned char *statep, const unsigned char *skp,
                           uint32_t layer);

#endif

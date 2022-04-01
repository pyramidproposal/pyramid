#include "format.h"

#include <stddef.h>
#include <stdint.h>

#include "address.h"
#include "convert.h"
#include "ext_algo.h"
#include "format.h"
#include "hash.h"
#include "params.h"
#include "string.h"

/*
  When deleted, all index bytes in sk are set to 0xFF.
  last_idxp, invalid_idxp, delete_sk keep this in mind,
  by discerning (PYR_FULL_HEIGHT % 8 ?= 0).
*/

/**
 * @brief Predicate: is idx the highest valid index?
 * @details Predicate: is idx the highest valid index?
 * @param[in] idx The index to answer for.
 * @return 0: not highest valid index; 1: highest valid index.
 */
int last_idxp(uint64_t idx) { return idx == LAST_VALID_IDX; }

/**
 * @brief Predicate: is idx an invalid index?
 * @details Predicate: is idx an invalid index?
 * @param[in] idx The index to answer for.
 * @return 0: not invalid index; 1: invalid index.
 */
int invalid_idxp(uint64_t idx) { return idx > LAST_VALID_IDX; }

/**
 * @brief Deletes the secret key.
 * @details Maximize the index (-1), overwrite the core part, overwrite external
 * part.
 * @param[inout] sk The secret key to delete.
 */
void delete_sk(unsigned char *sk) {
  /* Max index. */
  memset(sk + SK_IDX, 0xFF, PYR_IDX_BYTES);
  /* Overwrite SK core secrets. */
  memset(sk + SK_SSD, 0, NR_SSD * PYR_N);
  memset(sk + SK_PRF, 0, PYR_N);
  /* Overwrite SK public key copy. */
  memset(sk + SK_PRT, 0, PYR_N);
  memset(sk + SK_PSD, 0, PYR_N);
  /* Overwrite SK external part */
  memset(sk + PYR_SK_CORE_BYTES, 0, SK_EXT_ALGO_BYTES);
}

/**
 * @brief Create the message hash and its randomization element.
 * @details
 * @param[out] mhash The message hash.
 * @param[out] r The hash randomization element.
 * @param[in] sk The secret key.
 * @param[in] m The message to hash.
 * @param[in] mlen The length of the message.
 */
void sig_mhash(unsigned char *mhash, unsigned char *r, const unsigned char *sk,
               const unsigned char *m, size_t mlen) {
  uint32_t addr[8] = {0};
  uint64_t idx;

  idx = bytes_to_ull(sk + SK_IDX, PYR_IDX_BYTES);
  set_keypair_addr_bottom(addr, idx);
  set_type(addr, PYR_ADDR_TYPE_PRF);
  prf(r, sk + SK_PRF, sk + SK_PSD, addr);

  set_type(addr, PYR_ADDR_TYPE_HMSG);
  h_msg(mhash, r, sk + SK_PRT, addr, m, mlen);
}

void sig_base(unsigned char *sig, size_t *sig_len, const unsigned char *r,
              uint64_t idx) {
  *sig_len = 0;
  ull_to_bytes(sig + SIG_IDX, PYR_IDX_BYTES, idx);
  *sig_len += PYR_IDX_BYTES;
  memcpy(sig + SIG_RND, r, PYR_N);
  *sig_len += PYR_N;
}

void forget_refs(unsigned char *sk) {
  unsigned int i;
  int c;
  uint64_t idx_tree, idx;
  uint32_t idx_leaf;
  unsigned char *sk_ssd = sk + SK_SSD;
  const unsigned char *sk_psd = sk + SK_PSD;
  uint32_t addr[8] = {0};

  idx = bytes_to_ull(sk, PYR_IDX_BYTES);
  idx_tree = idx >> PYR_TREE_HEIGHT;
  idx_leaf = (idx & ((1 << PYR_TREE_HEIGHT) - 1));

  /* FSG uses layer, tree, keypair. */
  set_layer_addr(addr, 0);
  set_tree_addr(addr, idx_tree);
  set_keypair_addr(addr, idx_leaf);

  /* Lowest seed update (unconditional). */
  fsg(NULL, sk_ssd, sk_ssd, sk_psd, addr);
  c = (((uint64_t)idx_leaf + 1) >> PYR_TREE_HEIGHT) & 1;

  /* Upper seed update(s) (conditional).*/
  for (i = 1; (i < PYR_D) && c; i++) {
    idx_leaf = (idx_tree & ((1 << PYR_TREE_HEIGHT) - 1));
    idx_tree >>= PYR_TREE_HEIGHT;

    set_layer_addr(addr, i);
    set_tree_addr(addr, idx_tree);
    set_keypair_addr(addr, idx_leaf);

    fsg(NULL, sk_ssd + i * PYR_N, sk_ssd + i * PYR_N, sk_psd, addr);
    c = (((uint64_t)idx_leaf + 1) >> PYR_TREE_HEIGHT) & 1;
  }
}

#include "api.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "address.h"
#include "convert.h"
#include "ext_algo.h"
#include "format.h"
#include "hash.h"
#include "merkle.h"
#include "params.h"
#include "randombytes.h"
#include "thash.h"
#include "wots.h"

/*
 * Returns the length of a secret key, in bytes
 */
unsigned long long crypto_sign_secretkeybytes(void) {
  return CRYPTO_SECRETKEYBYTES;
}

/*
 * Returns the length of a public key, in bytes
 */
unsigned long long crypto_sign_publickeybytes(void) {
  return CRYPTO_PUBLICKEYBYTES;
}

/*
 * Returns the length of a signature, in bytes
 */
unsigned long long crypto_sign_bytes(void) { return CRYPTO_BYTES; }

/*
 * Returns the length of the seed required to generate a key pair, in bytes
 */
unsigned long long crypto_sign_seedbytes(void) { return CRYPTO_SEEDBYTES; }

/*
 * Generates a Pyramid key pair given a seed of length CRYPTO_SEEDBYTES.
 * Format sk: [index || SK.Seed || SK.PRF || PK.Seed || PK.Root || External state]
 * Format pk: [PK.Seed || PK.Root]
 * Future versions should include Algorithm OID in pk.
 */
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed) {
  int retval;
  /* Secret key */
  /* Index */
  memset(sk + SK_IDX, 0, PYR_IDX_BYTES);
  /* Secret seed */
  memcpy(sk + SK_SSD, seed, NR_SSD * PYR_N);
  seed += NR_SSD * PYR_N;
  /* PRF key */
  memcpy(sk + SK_PRF, seed, PYR_N);
  seed += PYR_N;
  /* Public seed */
  memcpy(sk + SK_PSD, seed, PYR_N);
  seed += PYR_N;

  /* Initialize hash function (sha/haraka) */
  initialize_hash_function(sk + SK_PSD, NULL);

  retval = ext_finalize_sk(sk + SK_PRT, sk);

  /* Public key */
  memcpy(pk + PK_RT, sk + SK_PRT, PYR_N);
  memcpy(pk + PK_SD, sk + SK_PSD, PYR_N);

  return retval;
}

/*
 * Samples random bytes & generates a Pyramid key pair.
 */
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk) {
  unsigned char seed[CRYPTO_SEEDBYTES];
  randombytes(seed, CRYPTO_SEEDBYTES);
  return crypto_sign_seed_keypair(pk, sk, seed);
}

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen, const uint8_t *m,
                          size_t mlen, uint8_t *sk) {
  unsigned char mhash[PYR_N];
  unsigned char r[PYR_N];
  uint64_t idx;
  int retval = 0;

  idx = bytes_to_ull(sk, PYR_IDX_BYTES);

  if (invalid_idxp(idx)) {
    *siglen = 0;
    return -2;
  }

  initialize_hash_function(sk + SK_PSD, NULL);

  sig_mhash(mhash, r, sk, m, mlen);
  sig_base(sig, siglen, r, idx);

  retval = ext_finalize_sig(sk, sig, siglen, mhash);

  if (last_idxp(idx)) {
    delete_sk(sk);
    return -1;
  } else {
    /* Algo-specific state update. */
    retval |= ext_update_state(sk);
    /* Forward secure state update. */
#ifdef FORWARD_SECURE
    forget_refs(sk);
#endif
    /* Post-algo: increment sk index by one. */
    ull_to_bytes(sk, PYR_IDX_BYTES, idx + 1);
  }

  return retval;
}

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen, const uint8_t *m,
                       size_t mlen, const uint8_t *pk) {
  const unsigned char *const pub_root = pk + PK_RT;
  const unsigned char *const pub_seed = pk + PK_SD;
  const unsigned char *const sig_idx = sig + SIG_IDX;
  const unsigned char *const sig_rnd = sig + SIG_RND;
  unsigned char wots_pk[PYR_WOTS_BYTES];
  unsigned char leaf[PYR_N];
  unsigned char root[PYR_N];
  unsigned char *mhash = root;
  unsigned long long idx_tree;
  unsigned int i;
  uint32_t idx_leaf;

  uint32_t hmsg_addr[8] = {0};
  uint32_t wots_addr[8] = {0};
  uint32_t wotspk_addr[8] = {0};
  uint32_t hashtree_addr[8] = {0};

  /* Set thash addresses and h_msg address types. */
  set_type(hmsg_addr, PYR_ADDR_TYPE_HMSG);
  set_type(wots_addr, PYR_ADDR_TYPE_WOTS);
  set_type(wotspk_addr, PYR_ADDR_TYPE_WOTSPK);
  set_type(hashtree_addr, PYR_ADDR_TYPE_HASHTREE);

  /* Algorithm OID should be checked here once present in pk. */

  /* Ensure correct signature length. */
  if (siglen != PYR_BYTES) {
    return -1;
  }

  /* Convert the index bytes from the signature to an integer. */
  idx_tree = bytes_to_ull(sig_idx, PYR_IDX_BYTES);

  /* Initialize the hash function (sha/haraka) */
  initialize_hash_function(pub_seed, NULL);

  /* For each hypertree layer. */
  for (i = 0; i < PYR_D; i++) {
    idx_leaf = (idx_tree & ((1 << PYR_TREE_HEIGHT) - 1));
    idx_tree >>= PYR_TREE_HEIGHT;

    set_layer_addr(wots_addr, i);
    set_layer_addr(wotspk_addr, i);
    set_layer_addr(hashtree_addr, i);

    set_tree_addr(wots_addr, idx_tree);
    set_tree_addr(wotspk_addr, idx_tree);
    set_tree_addr(hashtree_addr, idx_tree);

    /* The WOTS public key is only correct if the signature was correct. */
    set_keypair_addr(wots_addr, idx_leaf);

    if (i == 0) {
      /* Compute the message hash. */
      copy_keypair_addr(hmsg_addr, wots_addr);
      h_msg(mhash, sig_rnd, pub_root, hmsg_addr, m, mlen);
      sig += PYR_IDX_BYTES + PYR_N;
    }

    /* Initially, root = mhash, but on subsequent iterations it is the root
       of the subtree below the currently processed subtree. */
    wots_pk_from_sig(wots_pk, sig, root, pub_seed, wots_addr);
    sig += PYR_WOTS_BYTES;

    /* Compute the leaf node using the WOTS public key. */
    set_keypair_addr(wotspk_addr, idx_leaf);
    thash(leaf, wots_pk, PYR_WOTS_LEN, pub_seed, wotspk_addr);

    /* Compute the root node of this subtree. */
    compute_root(root, leaf, idx_leaf, sig, PYR_TREE_HEIGHT, pub_seed,
                 hashtree_addr);
    sig += PYR_TREE_HEIGHT * PYR_N;
  }

  /* Check if the root node equals the root node in the public key. */
  if (memcmp(root, pub_root, PYR_N)) {
    return -1;
  }

  return 0;
}

/**
 * Returns an array containing the signature followed by the message.
 */
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                unsigned char *sk) {
  size_t siglen;
  int retval;

  retval = crypto_sign_signature(sm, &siglen, m, (size_t)mlen, sk);

  memmove(sm + PYR_BYTES, m, mlen);
  *smlen = siglen + mlen;

  return retval;
}

/**
 * Verifies a given signature-message pair under a given public key.
 */
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk) {
  /* The API caller does not necessarily know what size a signature should be
     but Pyramid signatures are always exactly PYR_BYTES. */
  if (smlen < PYR_BYTES) {
    memset(m, 0, smlen);
    *mlen = 0;
    return -1;
  }

  *mlen = smlen - PYR_BYTES;

  if (crypto_sign_verify(sm, PYR_BYTES, sm + PYR_BYTES, (size_t)*mlen, pk)) {
    memset(m, 0, smlen);
    *mlen = 0;
    return -1;
  }

  /* If verification was successful, move the message to the right place. */
  memmove(m, sm + PYR_BYTES, *mlen);

  return 0;
}

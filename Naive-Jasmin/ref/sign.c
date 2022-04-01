#include <stddef.h>
#include <string.h>
#include <stdint.h>

#include "api.h"
#include "params.h"
#include "wots.h"
#include "hash.h"
#include "thash.h"
#include "address.h"
#include "randombytes.h"
#include "treehash.h"
#include "merkle.h"
#include "convert.h"
#include "format.h"

/*
 * Returns the length of a secret key, in bytes
 */
unsigned long long crypto_sign_secretkeybytes(void)
{
  return CRYPTO_SECRETKEYBYTES;
}

/*
 * Returns the length of a public key, in bytes
 */
unsigned long long crypto_sign_publickeybytes(void)
{
  return CRYPTO_PUBLICKEYBYTES;
}

/*
 * Returns the length of a signature, in bytes
 */
unsigned long long crypto_sign_bytes(void)
{
  return CRYPTO_BYTES;
}

/*
 * Returns the length of the seed required to generate a key pair, in bytes
 */
unsigned long long crypto_sign_seedbytes(void)
{
  return CRYPTO_SEEDBYTES;
}

/*
 * Generates an PYR key pair given a seed of length
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [PUB_SEED || root]
 */
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed)
{
  /* We do not need the auth path in key generation, but it simplifies the
     code to have just one treehash routine that computes both root and path
     in one function. */
  unsigned char auth_path[PYR_TREE_HEIGHT * PYR_N];
  uint32_t top_tree_addr[8] = {0};
    
  set_layer_addr(top_tree_addr, PYR_D - 1);

  /* Initialize index to 0. */
  memset(sk, 0, PYR_IDX_BYTES);
  sk += PYR_IDX_BYTES;

  /* Initialize SK_SEED and SK_PRF. */
  memcpy(sk, seed, 2 * PYR_N);

  /* Initialize PUB_SEED. */
  memcpy(sk + 3 * PYR_N, seed + 2 * PYR_N,  PYR_N);
  memcpy(pk + PYR_N, sk + 3*PYR_N, PYR_N);

  /* Compute root node of the top-most subtree. */
  treehash(pk, auth_path, sk, pk + PYR_N, 0, top_tree_addr);
  memcpy(sk + 2*PYR_N, pk, PYR_N);

  return 0;
}

/*
 * Generates an PYR key pair.
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [PUB_SEED || root]
 */
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
  unsigned char seed[CRYPTO_SEEDBYTES];
  randombytes(seed, CRYPTO_SEEDBYTES);
  crypto_sign_seed_keypair(pk, sk, seed);

  return 0;
}

int last_idxp(uint64_t idx){
  return idx == LAST_VALID_IDX;
}

int invalid_idxp(uint64_t idx){
  return idx > LAST_VALID_IDX;
}

static void delete_sk(unsigned char *sk) {
  memset(sk, 0xFF, PYR_IDX_BYTES);
  /* Overwrite the core of the key. */
  memset(sk + PYR_IDX_BYTES, 0, (PYR_SK_CORE_BYTES - PYR_IDX_BYTES));
}

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen,
                          const uint8_t *m, size_t mlen, uint8_t *sk)
{
  const unsigned char *sk_seed =  sk + PYR_IDX_BYTES;
  const unsigned char *sk_prf =   sk + PYR_IDX_BYTES + PYR_N;
  const unsigned char *pub_root = sk + PYR_IDX_BYTES + 2*PYR_N;
  const unsigned char *pub_seed = sk + PYR_IDX_BYTES + 3*PYR_N;
  unsigned char *R = sig + PYR_IDX_BYTES;

  unsigned char root[PYR_N];
  unsigned char *mhash = root;
  unsigned int i;
  uint32_t idx_leaf;
  uint64_t idx, idx_tree;

  uint32_t hmsg_addr[8] = {0};
  uint32_t wots_addr[8] = {0};
    
  set_type(wots_addr, PYR_ADDR_TYPE_WOTS);

  *siglen = 0;

  /* Read and use the current index from the secret key. */
  idx = (unsigned long)bytes_to_ull(sk, PYR_IDX_BYTES);
    
  /* Check if we can still sign with this sk.
   * If not, return -2
   * 
   * If this is the last possible signature (because the max index value 
   * is reached), production implementations should delete the secret key 
   * to prevent accidental further use.
   * 
   * XMSS uses the rule:
   * For the case of total tree height of 64 we do not use the last signature 
   * to be on the safe side (there is no index value left to indicate that the 
   * key is finished, hence external handling would be necessary).
   * 
   * In Pyramid, this is part of the rule for index length where 8 | index.
   */ 
  if(invalid_idxp(idx)){
    *siglen = 0;
    return -2;
  }
    
  memcpy(sig, sk, PYR_IDX_BYTES);

  /*************************************************************************
   * THIS IS WHERE PRODUCTION IMPLEMENTATIONS WOULD UPDATE THE SECRET KEY. *
   *************************************************************************/

  idx_tree = idx;

  set_type(wots_addr, PYR_ADDR_TYPE_WOTS);

  for (i = 0; i < PYR_D; i++) {
    idx_leaf = idx_tree & ((1 << PYR_TREE_HEIGHT)-1);
    idx_tree >>= PYR_TREE_HEIGHT;

    set_layer_addr(wots_addr, i);
    set_tree_addr(wots_addr, idx_tree);
    set_keypair_addr(wots_addr, idx_leaf);

    if(i == 0){
      /* Compute the digest randomization value. */
      copy_keypair_addr(hmsg_addr, wots_addr);
      set_type(hmsg_addr, PYR_ADDR_TYPE_PRF);
      prf(R, sk_prf, pub_seed, hmsg_addr);

      /* Compute the message hash. */
      set_type(hmsg_addr, PYR_ADDR_TYPE_HMSG);
      h_msg(mhash, R, pub_root, hmsg_addr, m, mlen);
      sig += PYR_IDX_BYTES + PYR_N;
    }

    /* Compute a WOTS signature. */
    /* Initially, root = mhash, but on subsequent iterations it is the root
       of the subtree below the currently processed subtree. */
    wots_sign(sig, root, sk_seed, pub_seed, wots_addr);
    sig += PYR_WOTS_BYTES;

    /* Compute the authentication path for the used WOTS leaf. */
    treehash(root, sig, sk_seed, pub_seed, idx_leaf, wots_addr);
    sig += PYR_TREE_HEIGHT*PYR_N;
  }

  *siglen = PYR_BYTES;

  if(last_idxp(idx)){
    delete_sk(sk);
    return -1;
  } else {
    /* Increment the index in the secret key. */
    ull_to_bytes(sk, PYR_IDX_BYTES, idx + 1);
  }

  return 0;
}

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen,
                       const uint8_t *m, size_t mlen, const uint8_t *pk)
{
  const unsigned char *pub_root = pk;
  const unsigned char *pub_seed = pk + PYR_N;
  const unsigned char *R = sig + PYR_IDX_BYTES;
  unsigned char wots_pk[PYR_WOTS_BYTES];
  unsigned char leaf[PYR_N];
  unsigned char root[PYR_N];
  unsigned char *mhash = root;
  unsigned long long idx = 0;
  unsigned int i;
  uint32_t idx_leaf;

  uint32_t hmsg_addr[8] = {0};
  uint32_t wots_addr[8] = {0};
  uint32_t wotspk_addr[8] = {0};
  uint32_t hashtree_addr[8] = {0};

  set_type(hmsg_addr, PYR_ADDR_TYPE_HMSG);  
  set_type(wots_addr, PYR_ADDR_TYPE_WOTS);
  set_type(wotspk_addr, PYR_ADDR_TYPE_WOTSPK);
  set_type(hashtree_addr, PYR_ADDR_TYPE_HASHTREE);

  /* Parameters should be checked here once present in pk. */
    
  if (siglen != PYR_BYTES) {
    return -1;
  }

  /* Convert the index bytes from the signature to an integer. */
  idx = bytes_to_ull(sig, PYR_IDX_BYTES);

  /* For each subtree.. */
  for (i = 0; i < PYR_D; i++) {
    idx_leaf = (idx & ((1 << PYR_TREE_HEIGHT)-1));
    idx >>= PYR_TREE_HEIGHT;

    set_layer_addr(wots_addr, i);
    set_layer_addr(wotspk_addr, i);
    set_layer_addr(hashtree_addr, i);

    set_tree_addr(wotspk_addr, idx);
    set_tree_addr(wots_addr, idx);
    set_tree_addr(hashtree_addr, idx);

    /* The WOTS public key is only correct if the signature was correct. */
    set_keypair_addr(wots_addr, idx_leaf);

    if(i == 0){
      /* Compute the message hash. */
      copy_keypair_addr(hmsg_addr, wots_addr);
      h_msg(mhash, R, pub_root, hmsg_addr, m, mlen);
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
    compute_root(root, leaf, idx_leaf, sig, PYR_TREE_HEIGHT, pub_seed, hashtree_addr);
    sig += PYR_TREE_HEIGHT*PYR_N;
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
                unsigned char *sk)
{
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
                     const unsigned char *pk)
{
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

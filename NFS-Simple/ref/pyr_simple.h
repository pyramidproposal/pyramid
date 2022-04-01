#ifndef PYR_SIMPLE_H
#define PYR_SIMPLE_H
#include <stddef.h>

/*
  This file is based on "xmss_simple: Simple and Memory-efficient XMSS^MT".
  Original code: https://github.com/HaruCrypto54/xmss_simple on Sep 15, 2021.
  Original code author:         Haruhisa Kosuge (HaruCrypto54).
  Co-author accompanying paper: Hidema Tanaka.
 */

/*
  Removed feature:
  If you want to speed up by increasing the state size,
  the original implementation allows you to  predefine
  WOTS_SIG_IN_STATE. Then, a state includes WOTS signature in each layer.
*/

/**
 * @def SAFE_MODE
 * Debug option included in the original source code.
 */
#define SAFE_MODE

/**
 * @def FAST_KEYGEN
 * If you want to reduce run time for key generation (2^{h/d-1} leaf
 * computations are reduced), predefine FAST_KEYGEN.
 */
#define FAST_KEYGEN

/**
 * @def SIG_XMSS_BYTES
 * WOTS signature, Authentication path.
 */
#define SIG_XMSS_BYTES (PYR_WOTS_BYTES + (PYR_TREE_HEIGHT * PYR_N))

/**
 * @def BDS_BYTES
 * Authentication path, Stack, Treehash, Keep.
 */
#define BDS_BYTES ( (PYR_TREE_HEIGHT * PYR_N)				\
		    + 1 + ( (PYR_TREE_HEIGHT - 1) * (PYR_N + 1) )	\
		    + ( PYR_TREE_HEIGHT * (PYR_N + 1 + PYR_IDX_BYTES) ) \
		    + ( (PYR_TREE_HEIGHT/2) * PYR_N) )

/**
 * @def MMT_BYTES
 * Authentication path, Stack, Treehash.
 */
#define MMT_BYTES ((PYR_TREE_HEIGHT * PYR_N)		   \
		   + 1 + ( PYR_TREE_HEIGHT * (PYR_N + 1) ) \
		   + ( PYR_TREE_HEIGHT * (1 + PYR_IDX_BYTES)))

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

#endif

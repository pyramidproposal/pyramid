#ifndef PYR_EXT_ALGO_H
#define PYR_EXT_ALGO_H
#include <stddef.h>

/* External algorithm header. */
#include "pyr_simple.h"

/**
 * @def FORWARD_SECURE
 * If defined, the external algorithm is forward-secure.
 */
#define FORWARD_SECURE

/**
 * @def PYR_SK_STATE_BYTES
 * The number of external algorithm state bytes within sk.
 */
#define SK_EXT_ALGO_BYTES XMSSMT_STATE_BYTES

/**
 * @def IDX_EXT_ALGO_MAX_VALID
 * The external algorithm may impose a bound on the maximum valid index.
 * If it does not impose such a bound, pick UINT64_MAX.
 * This is an experimental feature, may prove obsolete.
 */
#define IDX_EXT_ALGO_MAX_VALID UINT64_MAX

/**
 * @brief Finalizes the partially initialized sk.
 * @details Usage is twofold:
 * 1) Initilize the sk state of the external algorithm.
 * 2) Use external treehash to compute the root of the top tree.
 * @param[out] root The root of the top tree.
 * @param[inout] sk The partially initialized sk.
 * @return 0: Computation as expected; 1: Error (any).
 */
int ext_finalize_sk(unsigned char *root, unsigned char *sk);

/**
 * @brief Finalizes the signature.
 * @details Use the external algorithm state to finish the signature.
 * @param[in] sk The secret key that is being signed with.
 * @param[inout] sig The (partial) signature.
 * @param[inout] sig_len The (current) length of the signature.
 * @param[in] mhash The compressed message.
 * @return 0: Computation as expected; 1: Error (any).
 */
int ext_finalize_sig(const unsigned char *sk, unsigned char *sig,
                     size_t *sig_len, const unsigned char *mhash);

/**
 * @brief Update the external algorithm state within sk.
 * @details Update the external algorithm state within sk.
 * @param[inout] sk The secret key to update the external algorithm state of.
 * @return 0: Computation as expected; 1: Error (any).
 */
int ext_update_state(unsigned char *sk);

#endif

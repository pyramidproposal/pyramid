#include "ext_algo.h"

#include <stddef.h>

#include "pyr_simple.h"

int ext_finalize_sk(unsigned char *root, unsigned char *sk) {
  return xmssmt_finalize_sk(root, sk);
}

int ext_finalize_sig(const unsigned char *sk, unsigned char *sig,
                     size_t *sig_len, const unsigned char *mhash) {
  return xmssmt_ht_sign(sk, sig, sig_len, mhash);
}

int ext_update_state(unsigned char *sk) { return xmssmt_update_state(sk); }

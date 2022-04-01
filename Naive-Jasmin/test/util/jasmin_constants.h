#ifndef JASMIN_CONSTANTS_H
#define JASMIN_CONSTANTS_H

#include "../../ref/params.h"

#define PRF_INLEN (2 * PYR_N + 32)

/* Constant length part of variable lenght message. */
#define HMSG_CONST_INLEN_C (2 * PYR_N + 32)

/* thash input length */
#define TH1_INLEN (2 * PYR_N + PYR_ADDR_BYTES)
#define TH2_INLEN (3 * PYR_N + PYR_ADDR_BYTES)
#define THLEN_INLEN (PYR_ADDR_BYTES + (PYR_WOTS_LEN + 1) * PYR_N)

/* thash robust mask input length */
#define MASK_INLEN (PYR_N + PYR_ADDR_BYTES)

#endif

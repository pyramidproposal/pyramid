#ifndef PYR_PARAMS_H
#define PYR_PARAMS_H

/* Hash output length in bytes. */
#define PYR_N 16
/* Height of the hypertree. */
#define PYR_FULL_HEIGHT 20
/* Number of subtree layer. */
#define PYR_D 10
/* Winternitz parameter. */
#define PYR_WOTS_W 16

/* For clarity */
#define PYR_ADDR_BYTES 32

/* Index bytes in sk and signature. */
#define PYR_IDX_BYTES ((PYR_FULL_HEIGHT + 7) / 8)

/* WOTS parameters. */
#if PYR_WOTS_W == 256
#define PYR_WOTS_LOGW 8
#elif PYR_WOTS_W == 16
#define PYR_WOTS_LOGW 4
#else
#error PYR_WOTS_W assumed 16 or 256
#endif

#define PYR_WOTS_LEN1 (8 * PYR_N / PYR_WOTS_LOGW)

/* PYR_WOTS_LEN2 is floor(log(len_1 * (w - 1)) / log(w)) + 1; we precompute: */
#if PYR_WOTS_W == 256
#if PYR_N <= 1
#define PYR_WOTS_LEN2 1
#elif PYR_N <= 256
#define PYR_WOTS_LEN2 2
#else
#error Did not precompute PYR_WOTS_LEN2 for n outside {2, .., 256}
#endif
#elif PYR_WOTS_W == 16
#if PYR_N <= 8
#define PYR_WOTS_LEN2 2
#elif PYR_N <= 136
#define PYR_WOTS_LEN2 3
#elif PYR_N <= 256
#define PYR_WOTS_LEN2 4
#else
#error Did not precompute PYR_WOTS_LEN2 for n outside {2, .., 256}
#endif
#endif

#define PYR_WOTS_LEN (PYR_WOTS_LEN1 + PYR_WOTS_LEN2)
#define PYR_WOTS_BYTES (PYR_WOTS_LEN * PYR_N)
#define PYR_WOTS_PK_BYTES PYR_WOTS_BYTES

/* Subtree size. */
#define PYR_TREE_HEIGHT (PYR_FULL_HEIGHT / PYR_D)

#if PYR_TREE_HEIGHT * PYR_D != PYR_FULL_HEIGHT
#error PYR_D should always divide PYR_FULL_HEIGHT
#endif

#endif

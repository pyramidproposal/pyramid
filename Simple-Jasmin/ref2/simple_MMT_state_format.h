#ifndef PYR_MMT_STATE_FORMAT_H
#define PYR_MMT_STATE_FORMAT_H

#include "params.h"

/* MMT Treehash instance */
/* Size */
#define MMT_TH_FIN_SIZE (PYR_TREE_HEIGHT * 1)
#define MMT_TH_IDX_SIZE (PYR_TREE_HEIGHT * PYR_IDX_BYTES)
#define MMT_TH_SDA_SIZE (PYR_TREE_HEIGHT * PYR_N)

/* Offset */
#define MMT_TH_FIN 0
#define MMT_TH_IDX (MMT_TH_FIN + MMT_TH_FIN_SIZE)
#define MMT_TH_SDA (MMT_TH_IDX + MMT_TH_IDX_SIZE)
#define MMT_TH_SIZE (MMT_TH_SDA + MMT_TH_SDA_SIZE)

/* MMT Stack */
/* Size */
#define MMT_ST_SZE_SIZE 1
#define MMT_ST_NDE_SIZE (PYR_TREE_HEIGHT * PYR_N)
#define MMT_ST_HGT_SIZE PYR_TREE_HEIGHT
/* Offset */
#define MMT_ST_SZE 0
#define MMT_ST_NDE (MMT_ST_SZE + MMT_ST_SZE_SIZE)
#define MMT_ST_HGT (MMT_ST_NDE + MMT_ST_NDE_SIZE)
#define MMT_ST_SIZE (MMT_ST_HGT + MMT_ST_HGT_SIZE)

/* MMT State */
/* Size */
#define MMT_AUTH_SIZE (PYR_TREE_HEIGHT * PYR_N)
#define MMT_STCK_SIZE MMT_ST_SIZE
#define MMT_TREE_SIZE MMT_TH_SIZE
/* Offset */
#define MMT_AUTH 0
#define MMT_STCK (MMT_AUTH + MMT_AUTH_SIZE)
#define MMT_TREE (MMT_STCK + MMT_STCK_SIZE)
#define MMT_SIZE (MMT_TREE + MMT_TREE_SIZE)

#endif

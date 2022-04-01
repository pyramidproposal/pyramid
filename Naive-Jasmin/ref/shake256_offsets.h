#if !defined( SHAKE_OFFSETS_H_ )
#define SHAKE_OFFSETS_H_

/*
 * Offsets of various fields in the address structure when we use SHAKE256 as
 * the Pyramid hash function
 */

#define PYR_OFFSET_LAYER     3   /* The byte used to specify the Merkle tree layer */
#define PYR_OFFSET_TREE      8   /* The start of the 8 byte field used to specify the tree */
#define PYR_OFFSET_TYPE      19  /* The byte used to specify the hash type (reason) */
#define PYR_OFFSET_KP_ADDR3  21  /* The byte that Pyramid requires to match XMSS parameters. */
#define PYR_OFFSET_KP_ADDR2  22  /* The high byte (in SPX) used to specify the key pair (which one-time signature) */
#define PYR_OFFSET_KP_ADDR1  23  /* The low byte used to specify the key pair */
#define PYR_OFFSET_CHAIN_ADDR 27  /* The byte used to specify the chain address (which Winternitz chain) */
#define PYR_OFFSET_HASH_ADDR 31  /* The byte used to specify the hash address (where in the Winternitz chain) */
#define PYR_OFFSET_TREE_HGT  27  /* The byte used to specify the height of this node in the FORS or Merkle tree */
#define PYR_OFFSET_TREE_INDEX 28 /* The start of the 4 byte field used to specify the node in the FORS or Merkle tree */

#endif /* SHAKE_OFFSETS_H_ */

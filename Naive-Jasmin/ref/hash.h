#ifndef PYR_HASH_H
#define PYR_HASH_H

#include <stddef.h>
#include <stdint.h>

void prf(unsigned char *out, const unsigned char *key,
	 const unsigned char *pub_seed, const uint32_t addr[8]);

void h_msg(unsigned char *out, const unsigned char *R,
	   const unsigned char *root, const uint32_t addr[8],
	   const unsigned char *m, size_t mlen);
	    
/* Tests. */

void prf_jazz(unsigned char *out, const unsigned char *key,
	      const unsigned char *pub_seed, const uint32_t addr[8]);

void h_msg_jazz(unsigned char *out, const unsigned char *R,
		const unsigned char *root, const uint32_t addr[8],
		const unsigned char *m, size_t mlen);

#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "sha256.h"

/*
  https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
*/

const uint32_t H[8] = {0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
  0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19};

const uint32_t W[16] = {0x61626380, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000018};

const uint32_t MD[8] = {0xBA7816BF, 0x8F01CFEA, 0x414140DE, 0x5DAE2223,
  0xB00361A3, 0x96177A9C, 0xB410FF61, 0xF20015AD};

int main() 
{
  // Loop variable
  int neq;

  // Declare variables for test
  uint32_t H_[8];

  memcpy(H_, H, 8 * sizeof(uint32_t));
	
  // Test
  iterate_hash_jazz(W, H_);	
	
  neq = memcmp(MD, H_, 8 * sizeof(uint32_t));

  if(neq){
    printf("Test failed.\n");
  } else {
    printf("Test passed.\n");
  }

  return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "sha512.h"

/*
  https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
*/

const uint64_t H[8] = {0x6A09E667F3BCC908, 0xBB67AE8584CAA73B,
  0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
  0x510E527FADE682D1, 0x9B05688C2B3E6C1F,
  0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179};

const uint64_t W[16] = {
  0x6162638000000000, 0x0000000000000000, 0x0000000000000000,
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
  0x0000000000000018};

const uint64_t MD[8] = {0xDDAF35A193617ABA, 0xCC417349AE204131,
  0x12E6FA4E89A97EA2, 0x0A9EEEE64B55D39A,
  0x2192992A274FC1A8, 0x36BA3C23A3FEEBBD,
  0x454D4423643CE80E, 0x2A9AC94FA54CA49F};

int main() 
{
  // Loop variable
  int neq;

  // Declare variables for test
  uint64_t H_[8];

  memcpy(H_, H, 8 * sizeof(uint64_t));
	
  // Test
  iterate_hash_jazz(W, H_);	
	
  neq = memcmp(MD, H_, 8 * sizeof(uint64_t));

  if(neq){
    printf("Test failed.\n");
  } else {
    printf("Test passed.\n");
  }

  return 0;
}

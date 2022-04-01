#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "sha256.h"

/*
  https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
*/

#if INLEN == 3
const char *m = "abc";
const uint32_t MD[8] = {0xBA7816BF, 0x8F01CFEA, 0x414140DE, 0x5DAE2223,
  0xB00361A3, 0x96177A9C, 0xB410FF61, 0xF20015AD};
#elif INLEN == 56
const char *m = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
const uint32_t MD[8] = {0x248D6A61, 0xD20638B8, 0xE5C02693, 0x0C3E6039,
  0xA33CE459, 0x64FF2167, 0xF6ECEDD4, 0x19DB06C1};
#else
#error Not a valid test size.
#endif

void u32_to_bytes(unsigned char *out, uint64_t in)
{
  unsigned int i;
  for(i = 0; i < 4; i++){
    out[i] = (unsigned char)(in >> 8*(3 - i));
  }
}

int main() 
{
  // Loop variable
  int i, neq;

  // Declare variables for test
  unsigned char H_[32] = {0};
  unsigned char MD_[32];
	
  if(get_INLEN_jazz() != INLEN){
    fprintf(stderr, "Exit test: INLEN should be equal.\n");
    exit(-1);
  }

  for(i = 0; i < 8; i++){
    u32_to_bytes(MD_ + 4 * i, MD[i]);
  }
	
  // Test
  sha256_INLEN_jazz(H_, (const unsigned char*)m);
	
  neq = memcmp(MD_, H_, 8 * sizeof(uint32_t));

  if(neq){
    printf("Test failed.\n");
  } else {
    printf("Test passed.\n");
  }
	
  return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "sha512.h"

/*
  https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
*/

#if INLEN == 3
const char *m = "abc";
const uint64_t MD[8] = {0xDDAF35A193617ABA, 0xCC417349AE204131,
  0x12E6FA4E89A97EA2, 0x0A9EEEE64B55D39A,
  0x2192992A274FC1A8, 0x36BA3C23A3FEEBBD,
  0x454D4423643CE80E, 0x2A9AC94FA54CA49F};
#elif INLEN == 112
const char *m = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
const uint64_t MD[8] = {0x8E959B75DAE313DA, 0x8CF4F72814FC143F, 0x8F7779C6EB9F7FA1,
  0x7299AEADB6889018, 0x501D289E4900F7E4, 0x331B99DEC4B5433A,
  0xC7D329EEB6DD2654, 0x5E96E55B874BE909};
#else
#error Not a valid test size.
#endif

void u64_to_bytes(unsigned char *out, uint64_t in)
{
  unsigned int i;
  for(i = 0; i < 8; i++){
    out[i] = (unsigned char)(in >> 8*(7 - i));
  }
}

int main() 
{
  // Loop variable
  int i, neq;

  // Declare variables for test
  unsigned char H_[64] = {0};
  unsigned char MD_[64];
	
  if(get_INLEN_jazz() != INLEN){
    fprintf(stderr, "Exit test: INLEN should be equal.\n");
    exit(-1);
  }

  for(i = 0; i < 8; i++){
    u64_to_bytes(MD_ + 8 * i, MD[i]);
  }
	
  // Test
  sha512_INLEN_jazz(H_, (const unsigned char*)m);
	
  neq = memcmp(MD_, H_, 8 * sizeof(uint64_t));

  if(neq){
    printf("Test failed.\n");
  } else {
    printf("Test passed.\n");
  }

  return 0;
}

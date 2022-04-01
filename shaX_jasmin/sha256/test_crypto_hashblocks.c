#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "sha256.h"
#include "ref/api.h"
#include "randomtestdata.h"

/*
  Reference implementation (for random tests):
  https://bench.cr.yp.to/supercop.html
*/

#define NR_BLOCKS 2
#define MAX_TEST_BYTES ((NR_BLOCKS+1) * CRYPTO_BLOCKBYTES - 1)

int main() 
{
  // Loop variable
  unsigned int i, j;
  int neq, ret_c, ret_jazz;

  // Declare variables for test
  unsigned char state_bytes_c[CRYPTO_STATEBYTES];
  unsigned char state_bytes_jazz[CRYPTO_STATEBYTES];
  unsigned char in[MAX_TEST_BYTES];

  // Test 5k random inputs for each byte length.
  neq = 0;
  for(i = 0; i < 5000; i++){
    random_test_bytes(state_bytes_c, CRYPTO_STATEBYTES);
    memcpy(state_bytes_jazz, state_bytes_c, CRYPTO_STATEBYTES);
    random_test_bytes(in, MAX_TEST_BYTES);
	
    for(j = 0; j < CRYPTO_BLOCKBYTES; j++){
      ret_c = crypto_hashblocks(state_bytes_c, in, NR_BLOCKS * CRYPTO_BLOCKBYTES + j);
      ret_jazz = crypto_hashblocks_jazz(state_bytes_jazz, in, NR_BLOCKS * CRYPTO_BLOCKBYTES + j);
      
      neq |= memcmp(state_bytes_c, state_bytes_jazz, CRYPTO_STATEBYTES);
      neq |= (ret_c ^ ret_jazz);
    }
  }

  if(neq){
    printf("Test failed.\n");
  } else {
    printf("Test passed.\n");
  }

  return 0;
}

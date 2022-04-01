#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "sha512.h"
#include "ref/api.h"
#include "randomtestdata.h"

/*
  Reference implementation (for random tests):
  https://bench.cr.yp.to/supercop.html
*/

#define MAX_TEST_BYTES 5000

int main() 
{
  // Loop variable
  unsigned long long i;
  int neq, ret_c, ret_jazz;
  unsigned long long inlen;

  // Declare variables for test
  unsigned char out_c[CRYPTO_STATEBYTES];
  unsigned char out_jazz[CRYPTO_STATEBYTES];
  unsigned char in[MAX_TEST_BYTES];

  // Test MAX_TEST_BYTES random inputs
  neq = 0;
  for(i = 0; i < MAX_TEST_BYTES; i++){
    inlen = i;
    random_test_bytes(in, i);
	
    ret_c = crypto_hash(out_c, in, inlen);
    ret_jazz = crypto_hash_jazz(out_jazz, in, inlen);
      
    neq |= memcmp(out_c, out_jazz, CRYPTO_STATEBYTES);
    neq |= (ret_c ^ ret_jazz);
  }

  if(neq){
    printf("Test failed.\n");
  } else {
    printf("Test passed.\n");
  }

  return 0;
}

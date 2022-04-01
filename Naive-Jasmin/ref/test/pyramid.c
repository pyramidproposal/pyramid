#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../api.h"
#include "../params.h"
#include "../randombytes.h"

#undef OUTPUT_UPON_SUCCESS

#define PYR_MLEN       32
#define PYR_SIGNATURES ((LAST_VALID_IDX + 1 < 5000) ? LAST_VALID_IDX + 1 : 5000)
#define PYR_NR_FAIL    5

static inline int t_last_idxp(uint64_t idx){
  return idx == CRYPTO_MAX_VALID_IDX;
}

static inline int t_invalid_idxp(uint64_t idx){
  return idx > CRYPTO_MAX_VALID_IDX;
}

static inline int t_sk_deleted_correctlyp(const unsigned char sk[CRYPTO_SECRETKEYBYTES]){
  int retval = 0;
  size_t i;

  for(i = 0; (i < PYR_IDX_BYTES) && !retval; ++i){
    retval |= ~sk[i];
  }

  for(i = PYR_IDX_BYTES; (i < CRYPTO_SECRETKEYBYTES) && !retval; ++i){
    retval |= sk[i];
  }

  return retval;
}

static inline int t_mlen_correctp(size_t smlen){
  return smlen == PYR_MLEN;
}

static inline int t_smlen_correctp(size_t smlen){
  return smlen == PYR_BYTES + PYR_MLEN;
}

int main()
{
  int ret = 0;
  uint64_t i, j;

  /* Make stdout buffer more responsive. */
  setbuf(stdout, NULL);

  // Adapted the testing interface to use the API as much as possible. 
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char *m = malloc(PYR_MLEN);
  unsigned char *sm = malloc(CRYPTO_BYTES + PYR_MLEN);
  unsigned char *mout = malloc(CRYPTO_BYTES + PYR_MLEN);
  unsigned long long smlen;
  unsigned long long mlen;
  int retval;
  int last_index_correctly_reached = 0;
  uint64_t nr_fail_after_last_index = 0;

  randombytes(m, PYR_MLEN);

  printf("Generating keypair.. ");

  if (crypto_sign_keypair(pk, sk)) {
    printf("failed!\n");
    return -1;
  }
  printf("successful.\n");

  printf("Testing %lu signatures.. \n", (uint64_t)PYR_SIGNATURES);

  for (i = 0; (i < PYR_SIGNATURES) && !ret; i++) {
    printf("  - iteration #%lu:\n", i);

    retval = crypto_sign(sm, &smlen, m, PYR_MLEN, sk);

    switch(retval){
    case 0:
      if(t_invalid_idxp(i)){
	printf("Invalid index should result in error return value -2, got %d.\n", retval);
	ret |= -1;
      } else if(t_last_idxp(i)){
	printf("Last valid signature index should return warning value -1, got %d.\n", retval);
	ret |= -1;
      }
      break;
    case -1:
      if(t_invalid_idxp(i)){
	printf("Invalid index should result in error return value -2, got %d.\n", retval);
	ret |= -1;
      } else if(t_last_idxp(i)){
	printf("Reached the last index correctly!\n");
	last_index_correctly_reached = 1;
      } else {
	printf("Unknown failure, got %d without a last index.\n", retval);
	ret |= -1;
      }
      break;
    case -2:
      if(t_invalid_idxp(i)){
	printf("Unknown failure, return value %d correct,"
	       "but this should be unreachable.\n", retval);
      } else if(t_last_idxp(i)){
	printf("Last index should give a valid signature return value -1, got %d.\n", retval);
	ret |= -1;
      } else {
	printf("Unknown failure, got %d without an invalid index.\n", retval);
	ret |= -1;
      }
      break;
    default:
      printf("Received unrecognized return value %d.\n", retval);
      ret |= -1;
    }

    if (smlen != PYR_BYTES + PYR_MLEN) {
      printf("  X smlen incorrect [%llu != %u]!\n",
	     smlen, PYR_BYTES);
      ret = -1;
    }
    else {
#ifdef OUTPUT_UPON_SUCCESS
      printf("    smlen as expected [%llu].\n", smlen);
#endif
    }

    /* Test if signature is valid. */
    if (crypto_sign_open(mout, &mlen, sm, smlen, pk)) {
      printf("  X verification failed!\n");
      ret = -1;
    }
    else {
      #ifdef OUTPUT_UPON_SUCCESS
      printf("    verification succeeded.\n");
      #endif
    }

    /* Test if the correct message was recovered. */
    if (mlen != PYR_MLEN) {
      printf("  X mlen incorrect [%llu != %u]!\n", mlen, PYR_MLEN);
      ret = -1;
    }
    else {
      #ifdef OUTPUT_UPON_SUCCESS
      printf("    mlen as expected [%llu].\n", mlen);
      #endif
    }
    if (memcmp(m, mout, PYR_MLEN)) {
      printf("  X output message incorrect!\n");
      ret = -1;
    }
    else {
      #ifdef OUTPUT_UPON_SUCCESS
      printf("    output message as expected.\n");
      #endif
    }

    /* Test if signature is valid when validating in-place. */
    if (crypto_sign_open(sm, &mlen, sm, smlen, pk)) {
      printf("  X in-place verification failed!\n");
      ret = -1;
    }
    else {
      #ifdef OUTPUT_UPON_SUCCESS
      printf("    in-place verification succeeded.\n");
      #endif
    }

    /* Test if flipping bits invalidates the signature (it should). */

    /* Flip the first bit of the message. Should invalidate. */
    sm[smlen - 1] ^= 1;
    if (!crypto_sign_open(mout, &mlen, sm, smlen, pk)) {
      printf("  X flipping a bit of m DID NOT invalidate signature!\n");
      ret = -1;
    }
    else {
      #ifdef OUTPUT_UPON_SUCCESS
      printf("    flipping a bit of m invalidates signature.\n");
      #endif
    }
    sm[smlen - 1] ^= 1;

#ifdef PYR_TEST_INVALIDSIG
    int j;
    /* Flip one bit per hash; the signature is entirely hashes. */
    for (j = 0; j < (int)(smlen - PYR_MLEN); j += PYR_N) {
      sm[j] ^= 1;
      if (!crypto_sign_open(mout, &mlen, sm, smlen, pk)) {
	printf("  X flipping bit %d DID NOT invalidate sig + m!\n", j);
	sm[j] ^= 1;
	ret = -1;
	break;
      }
      sm[j] ^= 1;
    }
    if (j >= (int)(smlen - PYR_MLEN)) {
      #ifdef OUTPUT_UPON_SUCCESS
      printf("    changing any signature hash invalidates signature.\n");
      #endif
    }
#endif
  }

  /* For PYR_SIGNATURES > 0... */
  if(PYR_SIGNATURES - 1 < LAST_VALID_IDX){

    printf("Ending without having exhausted sk.\n");
    
  } else {
  
    ret |= (last_index_correctly_reached == 0);

    for(j = 0; (j < (uint64_t)PYR_NR_FAIL) && !ret; ++j){

      retval = crypto_sign(sm, &smlen, m, PYR_MLEN, sk);


      switch(retval){
      case 0:
      case -1:
	printf("Incorrectly returned %d after reaching the last index.\n", retval);
	ret |= -1;
	break;
      case -2:
	if(nr_fail_after_last_index == 0){
	  if(t_sk_deleted_correctlyp(sk)){
	    printf("==========================================================="
		   "================\n"
		   "Secret key deleted correctly; expecting %lu failure(s) "
		   "after this iteration.\n"
		   "==========================================================="
		   "================\n", (uint64_t)PYR_NR_FAIL);
	  } else {
	    printf("Return values -2, -1 in succession, but sk was not deleted correctly.\n");
	    ret |= -1;
	  }
	}
	if(nr_fail_after_last_index < (uint64_t)PYR_NR_FAIL){
	  if(t_sk_deleted_correctlyp(sk)){
	    printf("Successfully failed to sign: attempt %lu\n", nr_fail_after_last_index);
	    ++nr_fail_after_last_index;
	    if(nr_fail_after_last_index == (uint64_t)PYR_NR_FAIL){
	      printf("Correctness checked for %lu iterations,\n"
		     "Signature refusal checked for %lu iterations,\n"
		     "Test successfull.\n", i, (uint64_t)PYR_NR_FAIL);
	    }
	  } else {
	    printf("Return values -2, -1, ..., -1 in succession,"
		   "but sk is not deleted correctly anymore.\n");
	    ret |= -1;
	  }
	} else {
	  printf("Unknown failure, nr_fail_after_last_index > nr_fail.\n");
	  ret |= -1;
	}
	break;
      default:
	printf("Received unrecognized return value %d.\n", retval);
	ret |= -1;
      }

    }
  }

  free(m);
  free(sm);
  free(mout);

  return ret;
}

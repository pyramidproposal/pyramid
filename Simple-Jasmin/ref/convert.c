#include "convert.h"

#include <string.h>

/*
  This file contains most of the functions formerly in "util.c";
  It is limited to generic conversion functions.
 */

/**
 * Converts the value of 'in' to 'outlen' bytes in big-endian byte order.
 */
void ull_to_bytes(unsigned char *out, unsigned int outlen,
                  unsigned long long in) {
  int i;

  /* Iterate over out in decreasing order, for big-endianness. */
  for (i = outlen - 1; i >= 0; i--) {
    out[i] = in & 0xff;
    in = in >> 8;
  }
}

void u32_to_bytes(unsigned char *out, uint32_t in) {
  out[0] = (unsigned char)(in >> 24);
  out[1] = (unsigned char)(in >> 16);
  out[2] = (unsigned char)(in >> 8);
  out[3] = (unsigned char)in;
}

/**
 * Converts the inlen bytes in 'in' from big-endian byte order to an integer.
 */
unsigned long long bytes_to_ull(const unsigned char *in, unsigned int inlen) {
  unsigned long long retval = 0;
  unsigned int i;

  for (i = 0; i < inlen; i++) {
    retval |= ((unsigned long long)in[i]) << (8 * (inlen - 1 - i));
  }
  return retval;
}

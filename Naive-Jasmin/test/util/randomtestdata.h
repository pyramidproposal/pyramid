#ifndef RANDOMTESTDATA_H
#define RANDOMTESTDATA_H

#include <stddef.h>
#include <stdint.h>

void random_test_bytes(uint8_t* r, size_t count);
void random_test_16bit_blocks(uint16_t* r, size_t count);
void random_test_32bit_blocks(uint32_t* r, size_t count);
void random_test_64bit_blocks(uint64_t* r, size_t count);

#endif

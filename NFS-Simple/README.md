# Pyramid NFS-Simple

An implementation accompanying the Pyramid proposal. 
The implementation uses the [Simple](tree traversal algorithm implementation) 
by Haruhisa Kosuge and Hidema Tanaka, which is adapted to the Pyramid setting.

This is a non-scrutinized implementation and it should be **treated with care**.

## Testing

We have performed the test that is defined in `test/pyramid.c`
for the following **test** parameter sets, for every combination
of (SHAKE256, SHA256, Haraka) and (simple, robust). 
Note that `test/pyramid.c` performs at most 5000 signature iterations for every set.

| Name         | n   | h  | d   | w   |
|--------------|-----|----|-----|-----|
| test         | 16  | 16 | 4   | 16  |
| `test_n_24`  | 24  | 16 | 4   | 16  |
| `test_n_32`  | 32  | 16 | 4   | 16  |
| `test_d_1`   | 16  | 8  | 1   | 16  |
| `test_d_2`   | 16  | 8  | 2   | 16  |
| `test_d_3`   | 16  | 9  | 3   | 16  |
| `test_d_5`   | 16  | 25 | 5   | 16  |
| `test_d_6`   | 16  | 6  | 6   | 16  |
| `test_d_10`  | 16  | 20 | 10  | 16  |
| `test_w_256` | 16  | 4  | 2   | 256 |

The base parameter test is "test"; 
the other sets experiment with various combinations of parameters h, d.
Because test_w_256 requires a large number of F invocations, 
we have kept its tree small.

## System information

System information for the machine that was tested on:
```
Kernel: Linux 
Version (partial): 20.04.1-Ubuntu 
HW platform: x86_64
OS: GNU/Linux
```

gcc Version:
```
gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## Warnings

Whilst testing the aforementioned **test** parameter sets, 
we receive two compilation warnings, specifically for test_d_6:
```
pyr_simple.c: In function ‘bds_stateGen’:
pyr_simple.c:235:17: warning: comparison of unsigned expression < 0 is always false [-Wtype-limits]
  235 |   for (i = 0; i < PYR_TREE_HEIGHT - 1; i++) {
      |                 ^
pyr_simple.c: In function ‘bds_state_update’:
pyr_simple.c:329:46: warning: comparison of unsigned expression < 0 is always false [-Wtype-limits]
  329 |   if (!((idx_leaf >> (tau + 1)) & 1) && (tau < PYR_TREE_HEIGHT - 1)) {
      |                                              ^
```
These are expected: `PYR_TREE_HEIGHT = h/d = 1`; 
therefore the comparison is always false. 
The loop at line `357` initializes `auth[1], ..., auth[h/d-1]`; 
it should therefore run `0` times. 
The loop at line `523` stores `auth[tau]` in keep.
Keep is of size `0` when `PYR_TREE_HEIGHT = 1`;
the loop should run `0` times.

## HBS implementation sources

Pyramid aims for interoperability with SPHINCS+. 
As such, code for the addressing scheme, tweakable hash functions, etc., is inspired by it: 
> [SPHINCS+](https://github.com/sphincs/sphincsplus)

Pyramid FS-Simple uses a modified version of the Simple algorithm. 
The authors of Simple provide a non-forward-secure implementation, aimed at XMSS, at:
> [XMSS Simple](https://github.com/HaruCrypto54/xmss_simple)

Parts of the Pyramid reference code are from the original XMSS project, located at:
> [XMSS reference](https://github.com/XMSS/xmss-reference)

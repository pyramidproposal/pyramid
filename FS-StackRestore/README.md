# Pyramid FS-StackRestore (Stepping-stone)

An implementation accompanying the Pyramid proposal. 
The implementation does not use a dedicated tree traversal algorithm.
It is best characterized as an alteration to the basic XMSS
reference implementation, adapted to the Pyramid proposal. 

The primary difference with NFS-XMSS is that this implementation aims to achieve forward secrecy.
The two main changes from NFS-XMSS are as follows:
- Temporal SK_SSD references to achieve forward secrecy;
- Naive Treehash implementation that does not recompute left nodes;

Because Pyramid aims for cooperation with SPHINCS+, 
the adaptation to Pyramid uses the SHPINCS+ implementation as a starting point.

This is a non-scrutinized implementation and it should be **treated with care**.

## Testing

We have performed the test that is defined in `test/pyramid.c`
for the following **test** parameter sets, for every combination
of (SHAKE256, SHA256, Haraka) and (simple, robust). 
Note that `test/pyramid.c` performs at most 5000 signature iterations for every set.

| Name       | n   | h  | d   | w   |
|------------|-----|----|-----|-----|
| test       | 16  | 16 | 4   | 16  |
| test_n_24  | 24  | 16 | 4   | 16  |
| test_n_32  | 32  | 16 | 4   | 16  |
| test_d_1   | 16  | 8  | 1   | 16  |
| test_d_2   | 16  | 8  | 2   | 16  |
| test_d_3   | 16  | 9  | 3   | 16  |
| test_d_5   | 16  | 25 | 5   | 16  |
| test_d_6   | 16  | 6  | 6   | 16  |
| test_d_10  | 16  | 20 | 10  | 16  |
| test_w_256 | 16  | 4  | 2   | 256 |

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

## HBS implementation sources

Pyramid aims for interoperability with SPHINCS+. 
As such, code for the addressing scheme, tweakable hash functions, etc., is inspired by it: 
> [SPHINCS+](https://github.com/sphincs/sphincsplus)

Parts of the Pyramid reference code are from the original XMSS project, located at:
> [XMSS reference](https://github.com/XMSS/xmss-reference)

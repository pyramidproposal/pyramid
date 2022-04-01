# Pyramid FS-Simple in Jasmin

A Jasmin reproduction of the reference code that accompanies the 
first proposal for Pyramid. This particular reference implementation is referred to as FS-Simple.
The reference code uses a tree traversal algorithm called Simple, 
whilst implementing forward-security as we describe it in the first proposal.

## Jasmin version

Jasmin branch: [glob_array3](https://github.com/jasmin-lang/jasmin/commits/glob_array3)

Commit: 
[`aa031ef` by vbgl](https://github.com/jasmin-lang/jasmin/commit/aa031efa5c73f742443847fd19516baceaf753d4)

SHA: aa031efa5c73f742443847fd19516baceaf753d4

Date: Feb 5, 2022

Note that during general development, an earlier Jasmin version dating back to 
November 2021 was used. 

## Directory structure

The directory `.` is structured as follows:

- `asm/`: Jasmin-generated assembly files produced by `Makefile`.
- `Makefile`: recipes for tests for functional correctness.
- `ref/`: Pyramid FS-Simple C reference implementation, modified to test for functional correctness.
- `ref2/`: Pyramid FS-Simple Jasmin-based implementation. 
  Contains a C skeleton to call the functions 
  `crypto_sign_seed_keypair`, `crypto_sign_signature`, and `crypto_sign_verify`.
  These are exposed by `src/export.jazz`.
  Contains its own minimal `ref2/Makefile`
- `src/`: The `.jazz`, `.jahh` source files.
  - `params/`: `.jahh` test parameter sets. 
  A symbolic link in `src/` is used to compile against one of these.
- `test/`: Randomized tests for functional correctness, along with testing utilities.

The files in `src/` follow the following prefix convention:

- `fips202_`: Files that implement a part of FIPS 202, the SHA-3 standard.
  Most of these are a slightly modified version of the ones 
  provided by the Saber implementation in Jasmin; however, we currently only require SHAKE256.
- `pyr_`: Files that implement core constructions like WOTS, addressing, etc.
  these are more or less the same, regardless of the tree traversal algorithm that is used.
- `simple_`: Files that specifically implement the components of Simple, 
  examples including BDS and MMT. 
- `crypto_`: Implement Pyramid key generation, signing, and verification. 

Finally, the prefixes are also conforming the Saber implementation in Jasmin:

- `.jazz`: contain Jasmin export functions. Except for `src/export.jazz`, 
  these provide a wrapper around functions in `.jahh` files, used for testing. 
  Functions in `.jazz` files, suffixed `_jazz`, are exposed to C; 
  their function declaration is found in the analogous header files in `ref/`.
- `.jahh`: implement the logic from the C implementation in `ref/` in Jasmin.

## Makefile options

`Makefile`	is aimed towards building tests for functional correctness, 
i.e. every target `test/bin/test_*`.
Example usage scenarios, including those for `ref2/Makefile`, are as follows.
Instead of manually setting `PARAMS` and `THASH` in `Makefile`, 
one may also choose to include variable assignments directly from the command-line, 
i.e. `make THASH=simple PARAMS=pyramid-shake256-test target`.

Example 1: Test all individual Jasmin functions for parameter set `test_d_3` 
using the `simple` tweakable hash function instantiations:

```
# Set PARAMS = pyramid-shake256-test_d_3 in Makefile
# Set THASH  = simple in Makefile
make clean # Clear the previous tests.
make prep  # Create symbolic links for (PARAMS, THASH) combination.
make test  # Build every target in TESTS.
```

Example 2: Test Jasmin-based Pyramid FS-Simple for test parameter set `test_n_32` 
using the `robust` tweakable hash function instantiations:

```
# Set "PARAMS = pyramid-shake256-test_n_32" in Makefile
# Set "THASH  = robust" in Makefile
make clean # Clear the previous symbolic links.
make prep  # Create symbolic links for (PARAMS, THASH) combination.
cd ref2
make test
```

Example 3: Test the functional correctness of `crypto_sign_verify` located in `crypto_sign_verify.jahh`, which is exposed by `crypto.jazz`. 
Beforehand, ensure that there are no basic discrepancies between 
`src/pyramid_params.jahh` and `ref/params.h`

```
# Assume a clean environment with symbolic links prepared.
make test/bin/test_sanity_check
./test/bin/test_sanity_check # Output on stderr if accidental parameter mismatch.
make test/bin/test_crypto_sign_verify
./test/bin/test_crypto_sign_verify
```

## Implementational notes

### Design choice: clarity > performance

In certain cases, such as in `src/simple_BDS_state_update`, 
we store a variable `s_tau` on the stack to reuse it later. 
However, it may be more efficient to recompute tau when needed. 
In these cases, we prefer alignment with the C implementation over 
achieving a more optimized implementation. 
This is partially an artefact of the implementation being part of a *first proposal* for Pyramid.

Due to stateful HBS performance relying on the speed of the underlying hash function, 
as well as the number of leaf computations performed by the tree traversal algorithm, 
we currently prefer an "obvious" implementation over one that provides minor speedups
at the cost of an "intuitive" computation. 
Note that terms like "obvious" and "intuitive" are relative to the C implementation.

Another artefact of following the C implementation is the replication of the `&&` operator in C.
The original Simple implementation uses this operator to guard against 
empty stack pops (e.g. `while (stack.size > 0 && stack.nodeheight[stack.size - 1] == j)`), 
but also in places where short-circuiting is not strictly required.
Because Jasmin syntax provides a convenient way of implementing short-circuiting, 
i.e. using `while c_1 (e) c_2`, we have preserved these constructions. 
Implementations that "break free" from the C code should revert to alternatives using bit-wise
operations. 

### Design choice: specific typing

We opt to use the most specific typing possible. 

In files like `pyr_wots.jahh`, this results in the usage of `for` loops, 
even when one may have preferred not to unroll the entire loop. 
This is an artefact of subarrays requiring a constant starting index.

In files like `simple_treehash.jahh`, we do not enforce this rule due to an 
exponential number of loop iterations in Treehash.
As a result, additional copies are required to fetch/store nodes from/in the stack. 
Because the BDS/MMT states keep multiple Treehash instances, 
for which updates are based on the runtime variable `index`, 
excessive copies and/or additional register usage for indexing are a consequence.

Our choice to target specific typing, regardless of these issues, 
is hoped to increase the reusability of the project. 
If subarrays that use a runtime starting index are ever supported, 
we believe that our approach does lead to the greatest reusability of our code
for future endeavours for HBS in Jasmin.

## Testing

### Functional correctness tests

We have performed the tests for functional correctness in `src/`
for the following **test** parameter sets, for both `THASH=simple` and `THASH=robust`. 

| Name         | n   | h  | d   | w   |
|--------------|-----|----|-----|-----|
| `test`       | 16  | 16 | 4   | 16  |
| `test_n_24`  | 24  | 16 | 4   | 16  |
| `test_n_32`  | 32  | 16 | 4   | 16  |
| `test_d_1`   | 16  | 8  | 1   | 16  |
| `test_d_2`   | 16  | 8  | 2   | 16  |
| `test_d_3`   | 16  | 9  | 3   | 16  |
| `test_d_5`   | 16  | 25 | 5   | 16  |
| `test_d_6`   | 16  | 6  | 6   | 16  |
| `test_d_10`__†__ | 16  | 20 | 10  | 16  |
| `test_w_256` | 16  | 4  | 2   | 256 |

The base parameter test is "test"; 
the other sets experiment with various combinations of parameters h and d.
Because `test_w_256` requires a large number of F invocations, 
we have kept its tree small.

The set that is marked with __†__ prevents compilation. 
Attempts at compilation result in the following typing error: 

```
"src/simple_BDS_state_utils.jahh", line 33 (74-76):
typing error: the variable in has type u8[0], its array size should be positive
make: *** [Makefile:148: asm/jsimple_BDS_state.s] Error 1
```

The value of `BDS_ST_NDE_SIZE` is `0` when `h/d = 1`; 
the reserved stack space for BDS is `h/d - 1 = 0` nodes of size `n`.
Thus, the function `BDS_st_get_N` attempts to read `PYR_N` bytes from a `0`-byte region.
The function `BDS_st_get_N` is only used in the function `BDS_stack_pop`, 
which is used once in the file `simple_BDS_update_treehash.jahh`.
Part of the guard of the while loop that it is called in, is as follows:
```
size = BDS_st_get_size(state[BDS_STCK:BDS_ST_SIZE]);
if(size > 0) { /* state->stack->size > 0 */
	...
else {
	cont = 0; /* end loop */
	}
```
Hence, we would not expect the function to be problematic, 
given that it should not be called in the first place for a stack of constant size `0`. 
However, the problematic function is always attempted to be compiled because a while loop is used.

### Performance 

| Name                    | C (s)  |  C (r) | Jasmin (s) | Jasmin (r) |
|-------------------------|--------|--------|------------|------------|
| `XMSS-SHAKE_10_512`     |      ✓ |      ✓ |      ✓     |      ✓     |
| - `keygen`              | 200.18 | 319.87 | 339.28     | 530.81     |
| - `sign`                |  213.9 | 341.25 | 362.63     | 579.63     |
| - `verify`              |  14.85 |  24.45 |  25.55     |  41.52     |
| `XMSSMT-SHAKE_60/12_512`|      ✓ |      ✓ |      ✘     |      ✘     |
| - `keygen`              |   1.19 |   1.84 |      -     |      -     |
| - `sign`                | 451.77 | 699.30 |      -     |      -     |
| - `verify`              | 176.44 | 281.56 |      -     |      -     |

Depicted is the performance of the C and Jasmin implementations for simple (s)
& robust (r) thash strategies in seconds.
Compilation for parameter set `XMSSMT-SHAKE_60/12_512` causes a stack overflow
during compilation. 
This, along with the relatively slow performance compared to the C implementation, 
was expected given the tradeoff that was made between speed (-) and (anticipated)
code reusability (+).

## HBS implementation sources

Pyramid aims for interoperability with SPHINCS+. 
As such, code for the addressing scheme, tweakable hash functions, etc., is inspired by it: 
> [SPHINCS+](https://github.com/sphincs/sphincsplus)

Pyramid FS-Simple uses a modified version of the Simple algorithm. 
The authors of Simple provide a non-forward-secure implementation, aimed at XMSS, at:
> [XMSS Simple](https://github.com/HaruCrypto54/xmss_simple)

The Pyramid FS-Simple Jasmin implementation uses/modifies the SHAKE256 implementation 
that is part of the Saber implementation in Jasmin. 
The project structure is also inspired by the SABER-Jasmin project.
> [Saber Jasmin](https://github.com/MM45/SABER-Jasmin)

Parts of the Pyramid reference code are from the original XMSS project, located at:
> [XMSS reference](https://github.com/XMSS/xmss-reference)

Finally, we took the forward-secure XMSS implementation into consideration during development:
> [forward-secure XMSS](https://github.com/mkannwischer/xmssfs)

## Jasmin pointers

For this implementation, we considered (previous versions) of the following sources for information regarding Jasmin:

> [Getting started with jasmin](https://cryptojedi.org/programming/jasmin.shtml)

> [Jasmin compiler examples](https://github.com/jasmin-lang/jasmin/tree/glob_array3/compiler/examples)

> [Jasmin compiler tests](https://github.com/jasmin-lang/jasmin/tree/glob_array3/compiler/tests)

> [Jasmin Wiki](https://github.com/jasmin-lang/jasmin/wiki)

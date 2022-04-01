# Pyramid Naive-Jasmin

A Jasmin reproduction of the reference code that accompanies the first proposal for Pyramid also referred to as NFS-Naive.
The reference code does not use a tree traversal algorithm, 
nor does it achieve forward secrecy.

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
- `ref/`: Pyramid NFS-Naive C reference implementation, modified to test for functional correctness.
- `ref2/`: Pyramid NFS-Naive Jasmin-based implementation. 
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

Example 2: Test Jasmin-based Pyramid NFS-Naive for test parameter set `test_n_32` 
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

### Design choice: specific typing

we are aware of several drawbacks in the current implementation:

1. Code reusability: for every unique constant input length to SHAKE256, 
   we require an additional function definition.
2. Excessive copies: one may only compile sub-arrays with constant starting indices.
   We require additional copies from non-constant offsets to use sub-arrays 
   and preserve typing. 
3. Excessive unrolling: similar to excessive copies. We prefer `for` loops because 
   these align best with the reference implementation, and allow immediate use of sub-arrays
   using their `inline int` looping variable. However, we would prefer not to unroll 
   several `for` loops.
   Note that the functions in `src/utils.jahh` can- be easily changed to noinline alternatives.
4. Compound overloads: when writable- and read-only regions overlap for some invocations 
   of a function, we define an additional compound version. 
   This allows the Jasmin implementation to resemble the C implementation more closely, 
   at the cost of additional code duplication.

## HBS implementation sources

Pyramid aims for interoperability with SPHINCS+. 
As such, code for the addressing scheme, tweakable hash functions, etc., is inspired by it: 
> [SPHINCS+](https://github.com/sphincs/sphincsplus)

The Pyramid FS-Simple Jasmin implementation uses/modifies the SHAKE256 implementation 
that is part of the Saber implementation in Jasmin. 
The project structure is also inspired by the SABER-Jasmin project.
> [Saber Jasmin](https://github.com/MM45/SABER-Jasmin)

Parts of the Pyramid reference code are from the original XMSS project, located at:
> [XMSS reference](https://github.com/XMSS/xmss-reference)

## Jasmin pointers

For this implementation, we considered (previous versions) of the following sources for information regarding Jasmin:

> [Getting started with jasmin](https://cryptojedi.org/programming/jasmin.shtml)

> [Jasmin compiler examples](https://github.com/jasmin-lang/jasmin/tree/glob_array3/compiler/examples)

> [Jasmin compiler tests](https://github.com/jasmin-lang/jasmin/tree/glob_array3/compiler/tests)

> [Jasmin Wiki](https://github.com/jasmin-lang/jasmin/wiki)

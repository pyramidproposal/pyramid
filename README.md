# Disclaimer post-release

This branch is titled `post-release`.
By "post-release", we refer to the Jasmin version that we target. 

Recently, the first stable release of Jasmin was given, 
found at [Jasmin 21.0](https://github.com/jasmin-lang/jasmin/releases/tag/v21.0).
The release is accompanied by repository reorganization:

+ (previously known as) `master` becomes `release-21`;
+ (previously known as) `glob_array3` becomes `main`, which should lead to `release-22` in the future.

For this project, we targeted a specific version of `glob_array3`. 
When we target a newer version, we, therefore, target `main`.
We are unable to target `release-21`: 
various features of the branch `glob_array3` are missing; 
an example is the `require` system.

For `post-release`, we achieve compatibility with what is now the `main` branch of Jasmin.
To achieve this, we (currently) require one additional change to allow compilation, 
without using `-lazy-regalloc`.
We test this change for the Jasmin commit 
[b4ace6a](https://github.com/jasmin-lang/jasmin/commit/b4ace6ac0aca0895845d420ba8f7c8f80b35fa0a).
We consistently point out when we make this change using the comment:
```
/* "Fix" for compatibility with first release (typing error). */
```
An example may be found in the file 
`pyr_thash_shake256_robust.jahh` from line `65`.
When we try to re-assign the original value to `reg ptr u8[2*PYR_N] in`, 
we receive a typing error, because `in` is not mutable.
We circumvent this by renaming the parameter to `in_`, 
and immediately assigning `in = in_;`.

With the above change, we can compile the code. 
However, unlike compilation for (an older version of) `glob_array3`, we receive _a lot_ of warnings, 
specifically for two types of cases:

+ passing `stack` arrays to (inline) functions that expect type `reg ptr`
  (especially common in `.jazz` files);
+ lines such as `h[8 * i:8] = store64(h[8 * i:8], u);` with an implicit LEA instruction.

The second situation is often part of a `for` loop, thereby emitting repeated warnings.
Both types of warnings can be removed by adding explicit LEA instructions/assignments.
However, such changes do hurt the readability of the code a lot. 
For this reason, we currently choose not to alter these instances.

# utils.jahh

As noted in several places, the code size that we achieve is not practical.
This has to do with how sub-arrays currently function, 
but also with the copy functions that we include, e.g. `cpy_N` in `utils.jahh`.

For one, such copies do not (always/completely) need to be unrolled. 
However, unrolling allows us to not spill an additional register in the calling code.
We see this as a preferred solution when targeting reusability of the code, 
and readability also improves.
We also see the possibility of future Jasmin versions including dedicated constructions for 
`memcpy`- and `struct`-like features from C. 
Using these would certainly be preferable when present.

_If_ one were to still include their own copy functions, another approach could be preferable. 
Namely, one could align certain objects (keys, MMT state, etc.).
Alignment can be (de)constructed when loading/storing in `.jazz` files; 
this process is similar in purpose to the (de)serialization 
that happens in the C implementations for Simple.
This is allowed by the Jasmin feature referred to as "Flexible views of stack arrays" [ABB20].
Then, one could implement `cpy_N` as several 64-bit copies, for `N = 16, 24, 32` (bytes), 
while making sure that structure/key fields span multiples of 64 bits.
Most such fields are naturally multiples of `N` bytes (this is the main motivation).
Furthermore, you can directly store integers such as the 64-bit `idx` (secret key), 
without (big-endian) loads/stores from `utils.jahh`: these are only used upon (de)construction in the `.jazz` `export` function.

# Pyramid Proposal

This repository contains four implementations conform the Pyramid proposal.
The Pyramid proposal is part of the thesis titled 
"Proposal for a Pyramid scheme".
NFS-Naive and FS-StackRestore are stepping-stone implementations; 
these do not use a tree-traversal algorithm and are therefore not efficient.
However, these give a general impression of the functionality, 
and can be used to compare against the more involved implementations.

NFS-Simple and FS-Simple use the implementation for the Simple algorithm, 
which was originally defined in the context of XMSS. 
FS-Simple additionally slightly modifies the Simple algorithm to achieve a forward-secure 
implementation.

Finally, two Jasmin counterparts to the C implementations NFS-Naive and FS-Simple are included.
Their performance is not optimal, but these may provide a basis for further verification efforts for Pyramid.

The implementations contain individual `README`s that include build information.

## Contents

+ Pyramid implementations in C:
  - `NFS-Naive`
  - `FS-StackRestore`
  - `NFS-Simple`
  - `FS-Simple`
  - `common`
+ Pyramid implementations in Jasmin: 
  - `Simple` (FS-Simple)
  - `Naive`  (NFS-Naive)
+ Extra shaX in Jasmin (not thesis-related):
  - `shaX_jasmin` 

## References

[ABB20] Almeida, José Bacelar, et al. "The last mile: High-assurance and high-speed cryptographic implementations." _2020 IEEE Symposium on Security and Privacy (SP)_. IEEE, 2020.

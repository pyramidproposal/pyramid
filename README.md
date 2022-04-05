# Disclaimer

Please note that the Jasmin implementations in branch `main`
target- and were tested using an older version of the branch `glob_array3` of Jasmin.
The implementations in branch `post-release` aim for compatibility with a more recent version.

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

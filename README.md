Clang & LLVM Pass Collection
----------------------------

A collections of different passes for both LLVM and Clang.

LLVM & Clang evolve rapidly and the C++ API is not stable. This means that code that links against LLVM & Clang as libraries in version `X` may very well not compile or work in version`X+1`.

The `master` branch successfully builds against the latest release of LLVM (8.0) which can be found at: https://github.com/llvm/llvm-project/tree/release/8.x 

Every pass contains its own `README` detailing its use.

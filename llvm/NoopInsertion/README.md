No-op Insertion Backend Pass
----------------------------

The following is a backend `MachineFunctionPass` that inserts No-op instructions at the beginning of every basic block.

Support for `X86,ARM,AArch64`.

## Apply Patches
Backend passes need to be applied to the actual codebase and cannot be loaded through `opt`.

    $ git clone git@github.com:llvm/llvm-project.git
    $ cd llvm-project
    $ git checkout release/8.x # or relevant release
    $ git apply patches/release_8/*.patch

## Build
Build your LLVM release normally.

## Usage

    $ llc -mnoop-insertion

or 

    $ clang -fnoop-insertion

## Test
Backend passes should include their tests as part of the patches.


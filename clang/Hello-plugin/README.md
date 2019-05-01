Clang Plugin Pass
-----------------

A plugin pass for `clang` similar to `llvm` opt pass.

## Build

  $ make
  
## Usage

  $ clang -cc1 -load ./build/hello-plugin.so -plugin hello <c/cpp file>
  
## Test

  $ cd tests
  $ pytest test_runner.py

Clang LibTooling Hello World
----------------------------

A Hello World LibTooling tool which uses `clang`. 
More information on LibTooling can be found at https://clang.llvm.org/docs/LibTooling.html


## Build

  $ make
  
## Usage

  $ ./build/hello_tool <c/cpp file>

## Test

  $ cd tests
  $ bear make
  $ pytest test_runner.py

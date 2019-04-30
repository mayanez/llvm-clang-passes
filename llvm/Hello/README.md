Hello World LLVM Pass
---------------------

A plugin pass that can be dynamically loaded by `opt`.

## Build

  $ make
  
## Usage

  $ opt -load build/HelloPass.so -myhello <ir-file>

## Tests
  $ cd tests
  $ pytest test_runner.py

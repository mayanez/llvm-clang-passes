Statistics LLVM Pass
---------------------

A plugin pass that can be dynamically loaded by `opt`.
Outputs various statistics on IR.

## Build

    $ make
  
## Usage

    $ opt -load build/StatisticsPass.so -mystats <ir-file>

## Tests

    $ cd tests
    $ make
    $ pytest test_runner.py

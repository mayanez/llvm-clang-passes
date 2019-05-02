CloneFunctions LLVM Pass
---------------------

A plugin pass that can be dynamically loaded by `opt`.
Clones functions and places them in their own section.

## Build

    $ make
  
## Usage

    $ opt -load build/CloneFunctions.so -clonefuncs -S <ir-file> > <new-ir-file>

## Tests

    $ cd tests
    $ pytest test_runner.py

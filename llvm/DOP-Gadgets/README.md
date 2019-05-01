DOP Gadget LLVM Pass
---------------------
This code is based off https://github.com/melynx/DOP-StaticAssist.
It finds plausible DOP gadgets in a given program.

## Build

    $ make

## Usage

    $ opt -load build/DOPModule.so -dop-module <ir-file>

## Tests

    $ cd tests
    $ make
    $ pytest test_runner.py

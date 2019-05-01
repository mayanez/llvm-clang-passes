RecordDecl Padding LibTooling
-----------------------------

This tool outputs a CSV for each translation unit regarding information about paddings for each data structure (aka RecordDecl).

## Build

    $ make
  
## Usage

    $ ./build/calcpadding <c/cpp file>
  
## Test

    $ cd tests
    $ bear make
    $ pytest test_runner.py

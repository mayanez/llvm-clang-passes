CXX Class Dependency Graph
--------------------------

Traverse C++ program to generate class dependency graph.
Uses `libclang` python bindings to traverse `clang` AST.

## Usage

    $ ./cxxdepgraph.py compile_commands.json

## Tests
*TODO:* Add tests

    $ cd tests
    $ bear make
    $ python test_runner.py

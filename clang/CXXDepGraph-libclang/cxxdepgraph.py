#!/usr/bin/env python
"""
Usage:
    cxxdepgraph.py --help
    cxxdepgraph.py <compdb>
"""

import clang.cindex
import sys
import os
import networkx as nx
from networkx.drawing.nx_pydot import write_dot
from docopt import docopt

class ClassDependencies():
    def __init__(self):
        self.G = nx.DiGraph()

    def shouldSkipDecl(self, cursor):
        return '/usr' in os.path.dirname(str(cursor.location.file))

    def processClass(self, cursor):
        if not self.shouldSkipDecl(cursor):
            pClassNode = cursor.displayname
            self.G.add_node(pClassNode)

            for c in cursor.get_children():
                if c.kind == clang.cindex.CursorKind.CXX_BASE_SPECIFIER:
                    bClassNode = c.get_definition().displayname
                    self.G.add_node(bClassNode)
                    self.G.add_edge(pClassNode, bClassNode)

    def traverseAST(self, cursor):
        if (cursor.kind == clang.cindex.CursorKind.CLASS_DECL
                or cursor.kind == clang.cindex.CursorKind.STRUCT_DECL
                or cursor.kind == clang.cindex.CursorKind.CLASS_TEMPLATE):
            # if the current cursor is a class, class template or struct declaration,
            # we process it further ...
            self.processClass(cursor)

        for child_node in cursor.get_children():
            self.traverseAST(child_node)

    def parseTU(self, cursor):
        self.traverseAST(cursor)

    def writeDot(self):
        write_dot(self.G, 'deps.dot')

def getSyntaxOnlyCompileCommands(file_ccmds):
    syntax_args = ['clang-tool', '-fsyntax-only']
    for a in file_ccmds:
        t = list(a.arguments)
        t.remove(a.filename)
        syntax_args.extend(t)
    return syntax_args


if __name__ == '__main__':
    args = docopt(__doc__)
    compilation_database_path = os.path.dirname(args['<compdb>'])
    index = clang.cindex.Index.create()
    cd = ClassDependencies()
    compdb = clang.cindex.CompilationDatabase.fromDirectory(compilation_database_path)
    for cmds in compdb.getAllCompileCommands():
        source_file_path = cmds.filename
        file_ccmds = compdb.getCompileCommands(source_file_path)
        tu = index.parse(source_file_path, getSyntaxOnlyCompileCommands(file_ccmds))
        cd.parseTU(tu.cursor)
    cd.writeDot()

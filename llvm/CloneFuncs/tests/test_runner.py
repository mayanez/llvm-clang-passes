import os
import subprocess
import pandas as pd
import numpy as np
from io import StringIO
from llvmlite import binding as llvm
from llvmlite import ir as lc

PASS_SO_PATH = '../build/CloneFunctionsPass.so'

def run_llvmpass(srcfile, pass_so, pass_flag):
    assert os.path.exists(pass_so)
    cmd = 'opt -load {} {} -S {}'.format(pass_so, pass_flag, srcfile)
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return p

def test__clone():
    llvm.initialize()
    llvm.initialize_native_target()
    llvm.initialize_native_asmprinter()

    p = run_llvmpass('basic.ll', PASS_SO_PATH, '-clonefuncs')
    p.wait()
    pout = StringIO(p.stdout.read().decode('utf-8'))

    m = llvm.parse_assembly(pout.getvalue())
    m.verify()

    expected = set(['fact', 'fact.1'])
    actual = set([f.name for f in m.functions])

    assert expected == actual

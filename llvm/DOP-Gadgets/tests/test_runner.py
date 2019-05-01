import os
import subprocess
import pandas as pd
import numpy as np
from io import StringIO

PASS_SO_PATH = '../build/DOPModule.so'

def run_llvmpass(srcfile, pass_so, pass_flag):
    assert os.path.exists(pass_so)
    cmd = 'opt -load {} {} --disable-output {}'.format(pass_so, pass_flag, srcfile)
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return p

def test__basic():
    p = run_llvmpass('basic.ll', PASS_SO_PATH, '-dop-module')
    p.wait()

    pout = StringIO(p.stderr.read().decode('utf-8'))
    output = pd.read_csv(pout)

    assert output['total_loops'].iloc[[0]].values == 1
    assert output['interesting_loops'].iloc[[0]].values == 1
    assert output['mov_global'].iloc[[0]].values == 1
    assert output['deref_global'].iloc[[0]].values == 6
    assert output['arith_global'].iloc[[0]].values == 1
    assert output['logic_global'].iloc[[0]].values == 1
    assert output['cond_global'].iloc[[0]].values == 1

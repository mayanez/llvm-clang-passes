import os
import subprocess
import pandas as pd
import numpy as np
from io import StringIO

PASS_SO_PATH = '../build/StatisticsPass.so'

def run_llvmpass(srcfile, pass_so, pass_flag):
    assert os.path.exists(pass_so)
    cmd = 'opt -load {} {} --disable-output {}'.format(pass_so, pass_flag, srcfile)
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return p

def test__stats():
    p = run_llvmpass('basic.ll', PASS_SO_PATH, '-mystats')
    p.wait()

    pout = StringIO(p.stderr.read().decode('utf-8'))
    output = pd.read_csv(pout)

    assert output['totalPointers'].iloc[[0]].values == 5
    assert output['functionPointers'].iloc[[0]].values == 2
    assert output['calls'].iloc[[0]].values == 3
    assert output['indirectCalls'].iloc[[0]].values == 1
    assert output['storeInstructions'].iloc[[0]].values == 3


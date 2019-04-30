import os
import subprocess
import pandas as pd
import numpy as np

PASS_SO_PATH = '../build/HelloPass.so'

def run_llvmpass(srcfile, pass_so, pass_flag):
    assert os.path.exists(pass_so)
    cmd = 'opt -load {} {} --disable-output {}'.format(pass_so, pass_flag, srcfile)
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return p

def test__hello():
    p = run_llvmpass('basic.ll', PASS_SO_PATH, '-myhello')
    p.wait()

    expected = ['Hello: mult\n', 'Hello: fact\n']
    for i, l in enumerate(p.stderr):
        assert expected[i] == l.decode('utf-8')

def test__hello2():
    p = run_llvmpass('basic.ll', PASS_SO_PATH, '-myhello2')
    p.wait()

    expected = ['Hello: mult\n', 'Hello: fact\n']
    for i, l in enumerate(p.stderr):
        assert expected[i] == l.decode('utf-8')

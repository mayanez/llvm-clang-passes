import os
import subprocess
import pandas as pd
import numpy as np

BIN_PATH = '../build/hello_tool'

def run_tool(srcfile):
    assert os.path.exists(BIN_PATH)
    cmd = '{} ./{}'.format(BIN_PATH, srcfile)
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return p

def test__basic():
    p = run_tool('basic.c')
    p.wait()

    expected = ['Hello: main\n']
    for i, l in enumerate(p.stderr):
        assert expected[i] == l.decode('utf-8')

def test__basic_cxx():
    p = run_tool('basic.cpp')
    p.wait()

    expected = ['Hello: __bswap_16\n',
                'Hello: __bswap_32\n',
                'Hello: __bswap_64\n',
                'Hello: __uint16_identity\n',
                'Hello: __uint32_identity\n',
                'Hello: __uint64_identity\n',
                'Hello: abs\n',
                'Hello: abs\n',
                'Hello: abs\n',
                'Hello: abs\n',
                'Hello: abs\n',
                'Hello: div\n',
                'Hello: div\n',
                'Hello: isClass\n',
                'Hello: isClass\n',
                'Hello: allocate\n',
                'Hello: main\n']
    for i, l in enumerate(p.stderr):
      assert expected[i] == l.decode('utf-8')

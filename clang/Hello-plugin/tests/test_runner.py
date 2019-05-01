import os
import subprocess
import pandas as pd
import numpy as np

PLUGIN_SO_PATH = '../build/hello-plugin.so'
PLUGIN_FLAGS = '-hello'

def run_clang(srcfile, plugin_so, plugin_flags):
    assert os.path.exists(plugin_so)
    cmd = 'clang -Xclang -load -Xclang {} -Xclang -plugin -Xclang {} -Wno-unused-value -c {}'.format(plugin_so, plugin_flags, srcfile)
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return p

def test__basic():
    p = run_clang('basic.c', PLUGIN_SO_PATH, PLUGIN_FLAGS)
    p.wait()

    expected = ['Hello: main\n']
    for i, l in enumerate(p.stderr):
        assert expected[i] == l.decode('utf-8')

def test__basic_cxx():
    p = run_clang('basic.cpp', PLUGIN_SO_PATH, PLUGIN_FLAGS)
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

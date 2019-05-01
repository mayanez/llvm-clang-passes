import os
import subprocess
import pandas as pd
import numpy as np

BIN_PATH = '../build/calcpadding'

def run_calcpadding(srcfile):
    assert os.path.exists(BIN_PATH)
    cmd = '{} ./{}'.format(BIN_PATH, srcfile)
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.DEVNULL)

    return p

def test__no_pad_h():
    p = run_calcpadding('basic.c')
    p.wait()
    output = pd.read_csv('basic.c.padinfo')

    assert output['RecordName'].iloc[[5]].values == 'no_pad_h'
    assert output['TotalPadSize'].iloc[[5]].values == 0

def test__has_pad_h():
    p = run_calcpadding('basic.c')
    p.wait()
    output = pd.read_csv('basic.c.padinfo')

    assert output['RecordName'].iloc[[6]].values == 'has_pad_h'
    assert output['TotalPadSize'].iloc[[6]].values == 6

def test__no_pad():
    p = run_calcpadding('basic.c')
    p.wait()
    output = pd.read_csv('basic.c.padinfo')

    assert output['RecordName'].iloc[[7]].values == 'no_pad'
    assert output['TotalPadSize'].iloc[[7]].values == 0

def test__has_pad():
    p = run_calcpadding('basic.c')
    p.wait()
    output = pd.read_csv('basic.c.padinfo')

    assert output['RecordName'].iloc[[8]].values == 'has_pad'
    assert output['TotalPadSize'].iloc[[8]].values == 6

def test__has_pad_array():
    p = run_calcpadding('basic.c')
    p.wait()
    output = pd.read_csv('basic.c.padinfo')

    assert output['RecordName'].iloc[[9]].values == 'has_pad[10]'
    assert output['TotalPadSize'].iloc[[9]].values == 60

def test__NumPointers():
    p = run_calcpadding('basic.c')
    p.wait()
    output = pd.read_csv('basic.c.padinfo')

    assert output['NumPointers'].iloc[[6]].values == 0
    assert output['NumPointers'].iloc[[7]].values == 1

def test__MyClass():
    p = run_calcpadding('basic.cpp')
    p.wait()
    output = pd.read_csv('basic.cpp.padinfo')

    assert output['RecordName'].iloc[[22]].values == 'MyClass'
    assert output['TotalPadSize'].iloc[[22]].values == 4
    assert output['isVirtual'].iloc[[22]].values == 0


def test__MyClass2():
    p = run_calcpadding('basic.cpp')
    p.wait()
    output = pd.read_csv('basic.cpp.padinfo')

    assert output['RecordName'].iloc[[23]].values == 'MyClass2'
    assert output['TotalPadSize'].iloc[[23]].values == 4
    assert output['isVirtual'].iloc[[23]].values == 0

def test__MyClass3():
    p = run_calcpadding('basic.cpp')
    p.wait()
    output = pd.read_csv('basic.cpp.padinfo')

    assert output['RecordName'].iloc[[24]].values == 'MyClass3'
    assert output['TotalPadSize'].iloc[[24]].values == 4
    assert output['isVirtual'].iloc[[24]].values == 1

def test__MyClass4():
    p = run_calcpadding('basic.cpp')
    p.wait()
    output = pd.read_csv('basic.cpp.padinfo')

    assert output['RecordName'].iloc[[25]].values == 'MyClass4'
    assert output['TotalPadSize'].iloc[[25]].values == 0
    assert output['isVirtual'].iloc[[25]].values == 1

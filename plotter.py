# -*- coding: utf-8 -*-

from os.path import isfile, join
from os import listdir
from matplotlib import interactive
import os
import re
import sys
import numpy as np
import matplotlib.pyplot as plt

_msgPrefix = "[PLOTTER] - "

if len(sys.argv) < 4:
    print(
        "{}Usage: python3 ./plotter.py <folder> <motion1_name> <motion2_name> [<use_grid>]".format(_msgPrefix))
    exit(2)

try:
    folder = os.path.normpath(sys.argv[1])
    m1Name = sys.argv[2]
    m2Name = sys.argv[3]

    hmFile = folder+"/"+m1Name+"_"+m2Name+".hm"
    refFile = folder+"/"+m1Name+"_"+m2Name+".ref"

    data = np.loadtxt(hmFile, unpack=True)
    mark = np.loadtxt(refFile, unpack=True)

    if len(sys.argv) > 4 and re.match("[tT][a-zA-Z]*", sys.argv[4]):
        plt.style.use('seaborn-whitegrid')

    plt.figure("From "+m1Name.upper()+" to "+m2Name.upper())

    plt.xlabel(m1Name.upper())
    plt.ylabel(m2Name.upper())

    plt.scatter(mark[0], mark[1], s=5, c="black", alpha=0.3)
    plt.imshow(data, origin='lower', cmap="rainbow", interpolation="gaussian")
    plt.colorbar()

    plt.show()

except:
    print("{}Plot data not found".format(_msgPrefix))

# -*- coding: utf-8 -*-

import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import interactive

from os import listdir
from os.path import isfile, join

_msgPrefix = "[PLOTTER] - "


def plotHM(filePrefix):
    """Plots heatmap and winning frames from gived pair of motions"""

    dataFile = filePrefix+"__heatmap.plotData"
    markFile = filePrefix+"__refFrames.plotData"

    data = np.loadtxt(dataFile, unpack=True)
    mark = np.loadtxt(markFile, unpack=True)

    moNames = str(filePrefix).split(os.sep)[-1].split("_")
    f = plt.figure("From "+moNames[0].upper()+" to "+moNames[1].upper())

    plt.xlabel(moNames[0])
    plt.ylabel(moNames[1])

    plt.scatter(mark[0], mark[1], s=1, c="cyan", alpha=1, marker="D")
    plt.imshow(data, origin='lower', cmap="magma", interpolation="gaussian")
    plt.colorbar()

    f.show()


if __name__ == "__main__":
    try:
        if len(sys.argv) < 4:
            print(
                "{}Usage: python ./plotter.py <folder> <motion1_name> <motion2_name>".format(_msgPrefix))
            exit(2)

        folder = os.path.normpath(sys.argv[1])
        m1Name = sys.argv[2]
        m2Name = sys.argv[3]

        hmFile = folder+"/"+m1Name+"_"+m2Name+".hm"
        refFile = folder+"/"+m1Name+"_"+m2Name+".ref"

        data = np.loadtxt(hmFile, unpack=True)
        mark = np.loadtxt(refFile, unpack=True)

        f = plt.figure("From "+m1Name.upper()+" to "+m2Name.upper())

        plt.xlabel(m1Name)
        plt.ylabel(m2Name)

        plt.scatter(mark[0], mark[1], s=1, c="cyan", alpha=1, marker="D")
        plt.imshow(data, origin='lower', cmap="magma",
                   interpolation="gaussian")
        plt.colorbar()

        f.show()

        input("{}Press any key to close plots.".format(_msgPrefix))

    except:
        print("{}Plot data not found".format(_msgPrefix))

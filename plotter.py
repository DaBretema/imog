# -*- coding: utf-8 -*-

import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import interactive

from os import listdir
from os.path import isfile, join


def plot_diff(folder,filePrefix):
    """Plots heatmap and winning frames from gived pair of motions"""

    dataFile = folder+filePrefix+"__heatmap.txt"
    markFile = folder+filePrefix+"__refFrames.txt"

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



if __name__== "__main__":
    try:
        path = "./assets/plotdata/"
        files = [f for f in listdir(path) if isfile(join(path, f))]
        files = [f.split("__")[0] for f in files]
        files = list(dict.fromkeys(files))

        for f in files:
            plot_diff(path,f)

        input("Press any key to exit.")

    except FileNotFoundError:
        print("Don't found any plot at gived path, maybe don't exists!")

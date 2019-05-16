# -*- coding: utf-8 -*-

import sys
import numpy as np
import matplotlib.colors as cl
import matplotlib.pyplot as plt

filePrefix = sys.argv[1]
dataFile = filePrefix+"__heatmap.txt"
markFile = filePrefix+"__refFrames.txt"

data = np.loadtxt(dataFile, unpack=False)
axisDataMax =  len(data) if (len(data) > len(data[0])) else len(data[0])
print(axisDataMax)

mark = np.loadtxt(markFile, unpack=True)


plt.scatter(mark[0], mark[1], s=50, cmap="hot")
plt.imshow(data, origin='lower', extent=[0, axisDataMax, 0, axisDataMax])
plt.colorbar()
plt.show()

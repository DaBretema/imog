# -*- coding: utf-8 -*-

import sys
import numpy as np

import matplotlib.cm as cm
import matplotlib.pyplot as plt

data = np.loadtxt(sys.argv[1],unpack=True)
dMax = abs(data).max()
dLen = len(data)

# plt.imshow(data, cmap='hot', interpolation='quadric')
# plt.colorbar()
# plt.show()

plt.imshow(data, interpolation='bilinear', cmap=cm.RdYlGn,
                origin='lower', extent=[0, dLen, 0, dLen],
                vmax=dMax, vmin=0)
plt.colorbar()
plt.show()

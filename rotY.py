# -*- coding: utf-8 -*-

import sys


def rotY(x, y, z, direction=1.0):
    """
    Compute Y rotation based on X, Z and desired direction
    """

    meanXZ = (x + z) * 0.5
    newY = meanXZ + y
    newY *= direction

    return newY


try:

    # CLI arguments

    _x = float(sys.argv[1])
    _y = float(sys.argv[2])
    _z = float(sys.argv[3])

    _dirI = 1.0
    if len(sys.argv) > 4 and sys.argv[4].upper() == "L":
        _dirI = -1.0

    # Compute and print result

    Y = rotY(_x, _y, _z, _dirI)
    print(Y)

except ValueError:
    print("Check the values of X, Y and Z. They must be numbers.")

except IndexError:
    print("Expected 3 floats: X, Y, Z.")

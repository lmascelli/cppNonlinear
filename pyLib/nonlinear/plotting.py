import matplotlib.pyplot as plt
import numpy as np
from typing import List, Optional, Union
from . import core, pynl_bind


def vector_field(vf: List[List[float]], xrange: List[float],
                 yrange: List[float], points: List[int],
                 axes: Optional[plt.Axes]) -> None:
    x = np.linspace(xrange[0], xrange[1], points[0])
    y = np.linspace(yrange[0], yrange[1], points[1])

    for i in range(0, points[0]):
        for j in range(0, points[1]):
            if axes:
                axes.quiver(x[i], y[j],
                            vf[j*points[0]+i][0], vf[j*points[0]+i][1],
                            width=0.001,
                            scale=None,
                            pivot='middle')
            else:
                plt.quiver(x[i], y[j],
                           vf[j*points[0]+i][0], vf[j*points[0]+i][1],
                           width=0.001,
                           scale=None,
                           pivot='middle')


class Field_Options:
    def __init__(self, xrange: List[float], yrange: List[float],
                 sampling_points: List[int]):
        self.xrange = xrange
        self.yrange = yrange
        self.sampling_points = sampling_points

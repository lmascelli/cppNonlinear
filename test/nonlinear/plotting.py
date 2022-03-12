import matplotlib.pyplot as plt
import numpy as np
from typing import List, Optional
from . import core


def vector_field(vf: List[List[np.ndarray]], xrange: List[float],
                 yrange: List[float], points: List[int],
                 axes: Optional[plt.Axes]) -> None:
    x = np.linspace(xrange[0], xrange[1], points[0])
    y = np.linspace(yrange[0], yrange[1], points[1])

    for i in range(0, points[0]):
        for j in range(0, points[1]):
            if axes:
                axes.quiver(x[i], y[j], vf[j][i][0], vf[j][i][1],
                            width=0.001,
                            scale=None)
            else:
                plt.quiver(x[i], y[j], vf[j][i][0], vf[j][i][1],
                           width=0.001,
                           scale=None)


class Field_Options:
    def __init__(self, xrange: List[float], yrange: List[float],
                 sampling_points: List[int]):
        self.xrange = xrange
        self.yrange = yrange
        self.sampling_points = sampling_points


def test_vector_field(system: core.SystemDescriptor, params: List[float],
                      options: Field_Options):
    vf = core.vector_field(system, params,
                           options.xrange,
                           options.yrange,
                           options.sampling_points)
    fig, ax = plt.subplots()

    def on_key_press(event):
        if event.key == 'right' and params[0] < 1:
            print('Increasing alpha')
            params[0] += 0.1
        if event.key == 'left' and params[0] > 0:
            print('Decreasing alpha')
            params[0] -= 0.1
        if event.key == 'up' and params[1] < 1:
            print('Increasing betha')
            params[1] += 0.1
        if event.key == 'down' and params[1] > 0:
            print('Decreasing betha')
            params[1] -= 0.1
        vf = core.vector_field(system, params, options.xrange, options.yrange,
                               options.sampling_points)
        ax.clear()
        vector_field(vf, options.xrange, options.yrange,
                     options.sampling_points, ax)
        ax.set_title(f'a = {params[0]}, b = {params[1]}')
        fig.canvas.draw()

    fig.canvas.mpl_connect('key_press_event', on_key_press)
    vector_field(vf, options.xrange, options.yrange,
                 options.sampling_points, ax)
    ax.set_title(f'a = {params[0]}, b = {params[1]}')

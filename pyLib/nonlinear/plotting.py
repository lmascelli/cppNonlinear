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
                            scale=3000)
            else:
                plt.quiver(x[i], y[j],
                           vf[j*points[0]+i][0], vf[j*points[0]+i][1],
                           width=0.001,
                           scale=3000)


class Field_Options:
    def __init__(self, xrange: List[float], yrange: List[float],
                 sampling_points: List[int]):
        self.xrange = xrange
        self.yrange = yrange
        self.sampling_points = sampling_points


def test_vector_field(system: Union[core.SystemDescriptor,
                                    pynl_bind.CompiledSystemDescriptor], params: List[float],
                      options: Field_Options, compiled=False):

    vf: List[List[float]] = []
    if compiled:
        vf = pynl_bind.vector_field_2d(system, options.xrange, options.yrange,
                                       options.sampling_points, params)
    else:
        vf = core.vector_field(system, params, options.xrange, options.yrange,
                               options.sampling_points)
    fig, ax = plt.subplots()

    def replot():
        if compiled:
            vf = pynl_bind.vector_field_2d(system, options.xrange, options.yrange,
                                           options.sampling_points, params)
        else:
            vf = core.vector_field(system, params, options.xrange, options.yrange,
                                   options.sampling_points)
        ax.clear()
        vector_field(vf, options.xrange, options.yrange,
                     options.sampling_points, ax)
        ax.set_title(f'a = {params[0]}, b = {params[1]}')
        fig.canvas.draw()

    def on_key_press(event):
        if event.key == 'right' and params[0] < 1:
            print('Increasing alpha')
            params[0] += 0.1
            replot()
        if event.key == 'left' and params[0] > 0:
            print('Decreasing alpha')
            params[0] -= 0.1
            replot()
        if event.key == 'up' and params[1] < 1:
            print('Increasing betha')
            params[1] += 0.1
            replot()
        if event.key == 'down' and params[1] > 0:
            print('Decreasing betha')
            params[1] -= 0.1
            replot()

    fig.canvas.mpl_connect('key_press_event', on_key_press)
    vector_field(vf, options.xrange, options.yrange,
                 options.sampling_points, ax)
    ax.set_title(f'a = {params[0]}, b = {params[1]}')

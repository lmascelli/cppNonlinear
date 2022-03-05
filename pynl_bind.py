from typing import List
import pynl
import numpy as np
import matplotlib.pyplot as plt


def traiectory(system: object, t0: float, x0: List[float], params: List[float],
               n_points: int, step: float) -> List[List[float]]:
    return pynl.traiectory(system, t0, x0, params, n_points, step)


def vector_field_2d(system: object, xmin: float, xmax: float, ymin: float,
                    ymax: float, x_points: int, y_points: int,
                    params: List[float]) -> List[List[float]]:
    return pynl.vector_field_2d(system, xmin, xmax, ymin, ymax, x_points,
                                y_points, params)


def plot_transient(data: List[List[float]]) -> None:
    """
    TODO da aggiungere axis label e title
    """
    plt.subplot(3, 1, 1)
    plt.plot(data[0], data[1])
    plt.subplot(3, 1, 2)
    plt.plot(data[0], data[2])
    plt.subplot(3, 1, 3)
    plt.plot(data[1], data[2])
    plt.show()


def plot_vector_field(xrange: List[float], yrange: List[float], x_points: int,
                      y_points: int, data: List[List[float]]) -> None:
    x = np.linspace(xrange[0], xrange[1], x_points)
    y = np.linspace(yrange[0], yrange[1], y_points)
    plt.figure()
    plt.xlim(xrange)
    plt.ylim(yrange)
    for i, x_el in enumerate(x):
        for j, y_el in enumerate(y):
            plt.quiver(x_el, y_el, data[j*x_points + i][0],
                       data[j*x_points + i][1], units='width')

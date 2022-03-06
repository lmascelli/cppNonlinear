from typing import List
import matplotlib.pyplot as plt
import pynl_bind as nl
import numpy as np
import system_test


system = system_test.GetSystemDescriptor()


def systemf(x: List[float], params: List[float]) -> List[float]:
    a = 0.04
    b = 5
    c = 150
    if (x[0] > 30):
        return [0, 0]
    return [
        a * x[0] * x[0] + b*x[0] + c - x[1],
        params[0]*(params[1]*x[0] - x[1])
    ]


def vfp(xrange, yrange, points, params):
    x = np.linspace(xrange[0], xrange[1], points[0])
    y = np.linspace(yrange[0], yrange[1], points[1])

    ret = []

    for yel in x:
        for xel in y:
            ret.append(systemf([xel, yel], params))

    return ret


def printvf(vf: List[List[float]], xpoints: int):
    for i, el in enumerate(vf):
        print(el, end='')
        if (i+1) % xpoints == 0:
            print()


def test(xrange: List[float], yrange: List[float], points: List[float],
         params: List[float]) -> None:
    vf = nl.vector_field_2d(
        system, xrange[0], xrange[1], yrange[0], yrange[1], points[0], points[1], params)
    vtpc = vfp(xrange, yrange, points, params)
    nl.plot_vector_field(xrange, yrange, points[0], points[1], vf)
    printvf(vf, points[0])
    printvf(vtpc, points[0])
    plt.show()


test([-220, 50], [-100, 300], [30, 30], [0.1, 1])

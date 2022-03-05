from typing import List
import matplotlib.pyplot as plt
import pynl_bind as nl
import numpy as np
import system_test


system = system_test.GetSystemDescriptor()


def test(xrange: List[float], yrange: List[float], points: List[float],
         params: List[float]) -> None:
    vf = nl.vector_field_2d(
        system, xrange[0], xrange[1], yrange[0], yrange[1], points[0], points[1], params)
    nl.plot_vector_field(xrange, yrange, points[0], points[1], vf)
    plt.show()


test([-100, 30], [100, -100], [10, 10], [0.5, 0.5])

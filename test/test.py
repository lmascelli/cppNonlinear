import matplotlib.pyplot as plt
import numpy as np

from nonlinear import core, plotting, pynl_bind
from typing import List, Union, Tuple


######################################################
#
#               The system equations
#
######################################################

a = 0.04
b = 5.
c = 150.
d = -55
f = 4


def f(x: np.ndarray, params: List[float]) -> np.ndarray:

    return np.array([
        a*x[0]*x[0] + b*x[0]+c - x[1],
        params[0]*(params[1]*x[0] - x[1])
    ])


def f_jac(x: np.ndarray, params: List[float]) -> np.ndarray:
    return np.array([
        [2*a*x[0]+b, -1],
        [params[0]*params[1], -params[0]]
    ])


def map_f(x: np.ndarray, _: List[float]) -> np.ndarray:
    return np.array([d, x[1] + f])


def manifold(x: np.ndarray) -> int:
    return 1 if x[0] > 30 else 0


def equilibrium_points(params: List[float]) -> Tuple[bool, np.ndarray]:

    p1 = (params[1]-b)/(2*a)
    if (b*b - 2*b*params[1] + params[1]*params[1] - 4 * a*c) > 0:
        p2 = np.sqrt(b*b - 2*b*params[1] + params[1]*params[1] - 4 * a*c)/(2*a)

        return (
            True,
            np.array([
                [p1+p2, p1-p2],
                [(p1+p2)*params[1], (p1-p2)*params[1]]
            ]))
    else:
        print(f"for parameters {params} there are no equilibrium points")
        return (
            False,
            np.ndarray(())
        )


######################################################
#
#                   Analysis script
#
######################################################

params = [0.5, 0.2]
xrange = [-63., -59.]
yrange = [-6.4, -5.8]
sampling_points = [30, 30]


def test_vector_field(system: Union[core.SystemDescriptor,
                                    pynl_bind.CompiledSystemDescriptor],
                      compiled=False):

    fig, ax = plt.subplots()
    vf: List[List[float]] = []

    def get_pivot(eig: float) -> str:
        return 'tip' if eig < 0 else 'tail'

    def replot():
        if compiled:
            vf = pynl_bind.vector_field_2d(system, xrange, yrange,
                                           sampling_points, params)
        else:
            vf = core.vector_field(system, params, xrange, yrange,
                                   sampling_points)
        ax.clear()
        plotting.vector_field(vf, xrange, yrange,
                              sampling_points, ax)
        equ_exist, equ = equilibrium_points(params)
        if equ_exist:
            ax.scatter(equ[0, :], equ[1, :])

            w1, v1 = np.linalg.eig(f_jac(equ[:, 0], params))
            print(w1)
            w2, v2 = np.linalg.eig(f_jac(equ[:, 1], params))
            print(w2)

            ax.quiver(equ[0, 0], equ[1, 0], v1[0, 0],
                      v1[1, 0], 1, pivot=get_pivot(w1[0]),
                      width=0.003,
                      scale=None)
            ax.quiver(equ[0, 0], equ[1, 0], v1[0, 1],
                      v1[1, 1], 1, pivot=get_pivot(w1[1]),
                      width=0.003,
                      scale=None)
            ax.quiver(equ[0, 1], equ[1, 1], v2[0, 0],
                      v2[1, 0], 1, pivot=get_pivot(w2[0]),
                      width=0.003,
                      scale=None)
            ax.quiver(equ[0, 1], equ[1, 1], v2[0, 1],
                      v2[1, 1], 1, pivot=get_pivot(w2[1]),
                      width=0.003,
                      scale=None)

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
    replot()


def analysis():
    ################# System Descriptor ###################
    system = core.SystemDescriptor()
    system.add_function(f, core.EQUATION)
    system.add_function(map_f, core.MAP)
    system.manifold = manifold

    test_vector_field(system)

    plt.show()


def compiled_analysis():
    system = pynl_bind.GetSystemDescriptor()
    test_vector_field(system, True)
    plt.show()


compiled_analysis()

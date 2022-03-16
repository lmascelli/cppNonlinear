import matplotlib.pyplot as plt
from matplotlib.widgets import Button, Slider
import numpy as np

from nonlinear import core, plotting, pynl_bind
from typing import List, Union, Tuple


###############################################################################
#
#                             The system equations
#
###############################################################################

a = 0.04
b = 5.
c = 150.
d = -55
f = 4


def fun(x: np.ndarray, params: List[float]) -> np.ndarray:

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


###############################################################################
#
#                               Analysis script
#
###############################################################################

'''
The analysis of the system follows this steps:
    [X] plot of the vector field of the system varying with parameters
        and plot of the equilibrium points if any with associated jacobian's
        eigenvector and direction (given by eigenvalues)
    [ ] computation of the transient and eventually plot of it selecting the
        initial condition in the state space
    [ ]   plot of the eventual limit cycle
'''


class Analysis:
    def __init__(self,
                 system: Union[core.SystemDescriptor,
                               pynl_bind.CompiledSystemDescriptor],
                 params: List[float],
                 xrange: List[float],
                 yrange: List[float],
                 sampling_points: List[int],
                 compiled: bool = False):

        # system properties
        self.params = params
        self.system = system
        self.compiled = compiled
        self.x0 = [0., 0.]

        # vector field properties
        self.xrange = xrange
        self.yrange = yrange
        self.sampling_points = sampling_points

        # plot window properties
        self.fig, self.ax = plt.subplots()
        self.fig.canvas.mpl_connect('key_press_event', self.on_key_press)
        self.recompute = True

        self.plot_vf()

    def traiectory(self, points=10000, step=1e-3) -> List[List[float]]:
        if self.compiled:
            x0 = pynl_bind.traiectory(self.system, self.x0, self.params,
                                      points, step)
        else:
            x0 = core.traiectory(self.system, np.array(self.x0), self.params,
                                 points, step)
        return x0

    def get_pivot(self, eig: float) -> str:
        return 'tip' if eig < 0 else 'tail'

    def plot_connect(self):
        '''
        Restore the callbacks of the plot window after a redraw
        '''
        pass

    def plot_vf(self):
        if self.compiled:
            vf = pynl_bind.vector_field_2d(self.system, self.xrange, self.yrange,
                                           self.sampling_points, self.params)
        else:
            vf = core.vector_field(self.system,
                                   self.params,
                                   self.xrange,
                                   self.yrange,
                                   self.sampling_points)

        self.ax.clear()
        plotting.vector_field(vf, self.xrange, self.yrange,
                              self.sampling_points, self.ax)

        equ_exist, equ = equilibrium_points(self.params)
        if equ_exist:

            if equ[0, 0] > self.xrange[0] and equ[0, 0] < self.xrange[1] and \
               equ[1, 0] > self.yrange[0] and equ[1, 0] < self.yrange[1]:

                self.ax.scatter(equ[0, 0], equ[1, 0])
                w1, v1 = np.linalg.eig(f_jac(equ[:, 0], self.params))
                self.ax.quiver(equ[0, 0], equ[1, 0], v1[0, 0],
                               v1[1, 0], 1, pivot=self.get_pivot(w1[0]),
                               width=0.003,
                               scale=None)
                self.ax.quiver(equ[0, 0], equ[1, 0], v1[0, 1],
                               v1[1, 1], 1, pivot=self.get_pivot(w1[1]),
                               width=0.003,
                               scale=None)
                self.ax.quiver(equ[0, 0], equ[1, 0], -v1[0, 0],
                               -v1[1, 0], 1, pivot=self.get_pivot(w1[0]),
                               width=0.003,
                               scale=None)
                self.ax.quiver(equ[0, 0], equ[1, 0], -v1[0, 1],
                               -v1[1, 1], 1, pivot=self.get_pivot(w1[1]),
                               width=0.003,
                               scale=None)
            if equ[0, 1] > self.xrange[0] and equ[0, 1] < self.xrange[1] and \
               equ[1, 1] > self.yrange[0] and equ[1, 1] < self.yrange[1]:

                self.ax.scatter(equ[0, 1], equ[1, 1])
                w2, v2 = np.linalg.eig(f_jac(equ[:, 1], self.params))
                self.ax.quiver(equ[0, 1], equ[1, 1], v2[0, 0],
                               v2[1, 0], 1, pivot=self.get_pivot(w2[0]),
                               width=0.003,
                               scale=None)
                self.ax.quiver(equ[0, 1], equ[1, 1], v2[0, 1],
                               v2[1, 1], 1, pivot=self.get_pivot(w2[1]),
                               width=0.003,
                               scale=None)
                self.ax.quiver(equ[0, 1], equ[1, 1], -v2[0, 0],
                               -v2[1, 0], 1, pivot=self.get_pivot(w2[0]),
                               width=0.003,
                               scale=None)
                self.ax.quiver(equ[0, 1], equ[1, 1], -v2[0, 1],
                               -v2[1, 1], 1, pivot=self.get_pivot(w2[1]),
                               width=0.003,
                               scale=None)

        self.ax.set_title(f'a = {self.params[0]}, b = {self.params[1]}')
        self.fig.canvas.draw()
        self.ax.callbacks.connect('xlim_changed', self.on_xaxes_change)
        self.ax.callbacks.connect('ylim_changed', self.on_yaxes_change)

    def on_key_press(self, event):
        if event.key == 'right' and self.params[0] < 1:
            print('Increasing alpha')
            self.params[0] += 0.1
            self.plot_vf()
        if event.key == 'left' and self.params[0] > 0:
            print('Decreasing alpha')
            self.params[0] -= 0.1
            self.plot_vf()
        if event.key == 'up' and self.params[1] < 1:
            print('Increasing betha')
            self.params[1] += 0.1
            self.plot_vf()
        if event.key == 'down' and self.params[1] > 0:
            print('Decreasing betha')
            self.params[1] -= 0.1
            self.plot_vf()
        if event.key == 'a':
            self.recompute = not self.recompute
            print(f'Recumpute: {"ON" if self.recompute else "OFF"}')

    def on_xaxes_change(self, event):
        if self.recompute == True:
            self.xrange = event.get_xlim()
            self.plot_vf()

    def on_yaxes_change(self, event):
        if self.recompute == True:
            self.yrange = event.get_ylim()
            self.plot_vf()


# starting paramaters and properties
params = [0.5, 0.2]
xrange = [-90., 40.]
yrange = [-100, 30.]
sampling_points = [30, 30]


def analysis():

    ############################# System Descriptor ###########################

    system = core.SystemDescriptor()
    system.add_function(fun, core.EQUATION)
    system.add_function(map_f, core.MAP)
    system.manifold = manifold

    analysis_o = Analysis(system,
                          params,
                          xrange,
                          yrange,
                          sampling_points,
                          False)

    plt.show()


def compiled_analysis():
    system = pynl_bind.GetSystemDescriptor()
    analysis_o = Analysis(system,
                          params,
                          xrange,
                          yrange,
                          sampling_points,
                          True)
    plt.show()


compiled_analysis()

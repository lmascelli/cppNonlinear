import matplotlib.pyplot as plt
import numpy as np

from nonlinear import core, plotting
from typing import List


######################################################
#
#               The system equations
#
######################################################


def f(x: np.ndarray, params: List[float]) -> np.ndarray:
    a = 0.04
    b = 5.
    c = 150.
    return np.array([
        a * np.power(x[0], 2) + b * x[0] + c - x[1],
        params[0]*(params[1]*x[0]-x[1])
    ])


def map_f(x: np.ndarray, _: List[float]) -> np.ndarray:
    d = -55
    f = 4
    return np.array([d, x[1] + f])


def manifold(x: np.ndarray) -> int:
    return 1 if x[0] > 30 else 0


################# System Descriptor ##################
system = core.SystemDescriptor()
system.add_function(f, core.EQUATION)
system.add_function(map_f, core.MAP)
system.manifold = manifold


######################################################
#
#                   Analysis script
#
######################################################


### Vector field of the system varying a parameter ###

params = [0.5, 0.5]
xrange = [-100., 35.]
yrange = [-100., 100.]
sampling_points = [30, 30]

# plotting.test_vector_field(system, params,
#                           plotting.Field_Options(
#                               xrange, yrange, sampling_points))

x0 = np.array([-10., -150.])

t = core.traiectory(system, x0, params, 100000, 2e-4)
plt.plot(t[1])

plt.show()

from nonlinear import core
import numpy as np
from typing import List
import matplotlib.pyplot as plt

system = core.SystemDescriptor()


def f(x: np.ndarray, params: List[float]) -> np.ndarray:
    res = np.array([
        x[1, 0],
        params[0]*x[0, 0],
    ]).reshape(2, 1)
    return res


def kj(x: np.ndarray, params: List[float]) -> np.ndarray:
    return np.array(
        [
            2*x[0] + 3*x[1],
            1*x[0] + 5*x[1]
        ]
    )


system.add_function(f, core.EQUATION)
system.manifold = lambda x: 0
x0 = np.array([1., 0.]).reshape(2, 1)
params = [-1.]
T = 2.0*np.pi
step = 1e-3
n_steps = int(T/step)

tm = core.transition_matrix(system, x0, params, T, step)
t = core.traiectory(system, x0, params, n_steps, step)
print(np.exp(tm))

plt.plot(t[0, :], t[1, :])
plt.show()

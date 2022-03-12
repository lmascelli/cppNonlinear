import numpy as np
from typing import List, Callable, Tuple

# Function Types
EQUATION = 0
MAP = 1

INTEGRATION_METHOD = "Euler"

# A prototype for a function ruling the system
# It must define the function and the type:
# - EQUATION: and ODE function
# - MAP: a mapping function


class System_Function:
    def __init__(self, f: Callable[[np.ndarray, List[float]], np.ndarray],
                 type: int):
        self.f = f
        self.type = type

# A class for managing the functions ruling the system
# Used in the following analysis functions


class SystemDescriptor:

    functions: List[System_Function] = []

    manifold: Callable[[np.ndarray], int]

    def __init__(self, function_number: int = 1) -> None:
        self.function_number = function_number

    def add_function(self,
                     function: Callable[[np.ndarray, List[float]],
                                        np.ndarray],
                     function_type: int) -> None:
        self.functions.append(System_Function(function, function_type))
        self.function_number += 1


# Integrate a time step of a System
# TODO implement also Runge-Kutta algorithm


def integrate(system: SystemDescriptor, x0: np.ndarray, params: List[float],
              step: float) -> Tuple[np.ndarray, int]:
    """
    Euler ODE integration
    """
    previous_label = system.manifold(x0)
    if system.functions[previous_label].type == MAP:
        x0 = system.functions[previous_label].f(x0, params)
    else:
        x0 += system.functions[previous_label].f(x0, params) * step

    after_label = system.manifold(x0)

    if previous_label != after_label:
        return (x0, after_label)
    else:
        return (x0, -1)


# Integrate and store N_STEPS of a system starting from X0
def traiectory(system: SystemDescriptor, x0: np.ndarray, params: List[float],
               n_steps: int, step: float) -> np.ndarray:
    t_ret = np.zeros(shape=(2, n_steps))
    t_ret[0, 0] = x0[0]
    t_ret[1, 0] = x0[1]
    for i in range(1, n_steps):
        x0, event = integrate(system, x0, params, step)
        if event >= 0:
            pass
        t_ret[0, i] = x0[0]
        t_ret[1, i] = x0[1]
    return t_ret


# Compute the vector field of a 2D system in a given param space
def vector_field(system: SystemDescriptor, params: List[float],
                 xrange: List[float], yrange: List[float],
                 points: List[int]) -> List[List[np.ndarray]]:
    x = np.linspace(xrange[0], xrange[1], points[0])
    y = np.linspace(yrange[0], yrange[1], points[1])

    ret = []

    for j in range(0, points[1]):
        row = []
        for i in range(0, points[0]):
            point = np.array([x[i], y[j]])
            label = system.manifold(point)
            if system.functions[label].type == EQUATION:
                row.append(system.functions[label].f(point, params))
            else:
                row.append(np.array([0, 0]))
        ret.append(row)

    return ret


def shooting(system: SystemDescriptor, x0: np.ndarray, params: List[float],
             T: float, step: float) -> None:
    pass

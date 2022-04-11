import numpy as np
from typing import List, Callable, Optional, Tuple


# custom defined machine precision value
EPSILON = 1e-9

######################################################
#
#               CLASSES AND STRUCTURES
#
######################################################


# A prototype for a function ruling the system
# It must define the function and the type:
# - EQUATION: and ODE function
# - MAP: a mapping function

# Function Types
EQUATION = 0
MAP = 1

INTEGRATION_METHOD = "Euler"


class System_Function:
    def __init__(self, f: Callable[[np.ndarray, List[float]], np.ndarray],
                 type: int,
                 jacobian: Optional[Callable[[np.ndarray, List[float]],
                                             np.ndarray]] = None):
        self.f = f
        self.type = type
        self.jacobian = jacobian

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
                     function_type: int,
                     jacobian: Optional[Callable[[np.ndarray, List[float]],
                                        np.ndarray]] = None) -> None:
        self.functions.append(System_Function(
            function, function_type, jacobian))
        self.function_number += 1


# That's a class computing and  holding informations about the stability of an
# equilibrium point

class EquPointAnalisys:
    UNKNOWN = -1
    STABLE = 0
    UNSTABLE = 1
    SADLE = 2
    CENTER = 3
    STABLE_FOCUS = 4
    UNSTABLE_FOCUS = 5

    EQU_STR = {
        UNKNOWN: "unkown",
        STABLE: "stable",
        UNSTABLE: "unstable",
        SADLE: "sadle",
        CENTER: "center",
        STABLE_FOCUS: "stable focus",
        UNSTABLE_FOCUS: "unstable focus",
    }

    def __init__(self, jac: Callable[[np.ndarray, List[float]],
                                     np.ndarray],
                 point: np.ndarray, params: List[float]):
        self.point = point
        self.eigenvalues, self.eigenvectors = np.linalg.eig(jac(point, params))

        # TODO this script classify only 2D systems, maybe it can be
        # enhanced with higher dimensional systems

        if np.iscomplex(self.eigenvalues):                      # λ = A + iB
            if np.abs(np.real(self.eigenvalues[0]) < EPSILON):  # A = 0
                self.type = EquPointAnalisys.CENTER
            elif np.real(self.eigenvalues[0]) > 0:
                self.type = EquPointAnalisys.UNSTABLE_FOCUS     # A > 0
            else:
                self.type = EquPointAnalisys.STABLE_FOCUS       # A < 0

        elif self.eigenvalues[0] > 0 and \
                self.eigenvalues[1] > 0:                        # λ1 > 0 λ2 > 0
            self.type = EquPointAnalisys.UNSTABLE

        elif self.eigenvalues[0] > 0 and \
                self.eigenvalues[1] < 0:                        # λ1 > 0 λ2 < 0
            self.type = EquPointAnalisys.SADLE

        elif self.eigenvalues[0] < 0 and \
                self.eigenvalues[1] > 0:                        # λ1 < 0 λ2 > 0
            self.type = EquPointAnalisys.SADLE

        elif self.eigenvalues[0] < 0 and \
                self.eigenvalues[1] < 0:                        # λ1 < 0 λ2 < 0
            self.type = EquPointAnalisys.STABLE

        else:
            self.type = EquPointAnalisys.UNKNOWN


######################################################
#
#                     FUNCTIONS
#
######################################################

# Integrate a time step of a System
# TODO implement also Runge-Kutta algorithm


def jacobian(fun: Callable[[np.ndarray, List[float]], np.ndarray],
             params: List[float],  x: np.ndarray, step: float = 1e-9) -> np.ndarray:

    ret = np.zeros(shape=(x.size, x.size))
    x = np.reshape(x, (x.size, 1))

    def f(y, h):
        res = (fun(y+h, params) - fun(y-h, params))/(2*step)
        return np.reshape(res, (1, x.size))

    for i in range(x.size):
        h = np.zeros(shape=(x.size, 1))
        h[i, 0] = 1*step
        ret[i, :] = f(x, h)
        pass
    return ret.T


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
                 points: List[int]) -> np.ndarray:
    x = np.linspace(xrange[0], xrange[1], points[0])
    y = np.linspace(yrange[0], yrange[1], points[1])

    ret = np.ndarray(shape=(2, points[0], points[1]))

    for j in range(0, points[1]):
        for i in range(0, points[0]):
            point = np.array([x[i], y[j]])
            label = system.manifold(point)
            if system.functions[label].type == EQUATION:
                v = system.functions[label].f(point, params)
                ret[:, j, i] = np.array([v[0], v[1]])
            else:
                ret[:, j, i] = np.zeros(2)
    return ret


def transition_matrix(system: SystemDescriptor, x0: np.ndarray, params: List[float],
                      T: float, step: float) -> np.ndarray:
    system_size = x0.shape[0]
    print(f'system_size: {system_size}')  # DEBUG

    ret = np.ones(shape=(system_size, system_size))
    acc = np.zeros(shape=(system_size, system_size))

    n_steps: int = int(np.floor(T/step))
    print(f'overcoming the period by t: {step*n_steps-T}')

    current_flag = system.manifold(x0)
    for i in range(n_steps):
        jac = system.functions[current_flag].jacobian
        if not jac is None:
            acc += jac(x0, params)*step
        else:
            acc += jacobian(system.functions[current_flag].f, params, x0)*step
        x0, type = integrate(system, x0, params, 1e-3)
        if not type == current_flag:
            # saltation matrix
            pass
    return ret*acc

import numpy as np
import matplotlib.pyplot as plt

from nonlinear import core, plotting
from typing import List


def f(x: np.ndarray, _: List[float]) -> np.ndarray:
    return np.array([
        x[0] - x[1] - np.power(x[0], 3) - x[0] * np.power(x[1], 2),
        x[0] + x[1] - np.power(x[0], 2) * x[1] - np.power(x[1], 3)
    ])


def manifold(_: np.ndarray) -> int:
    return 0


system = core.SystemDescriptor()
system.add_function(f, core.EQUATION)
system.manifold = manifold

plotting.test_vector_field(system, [0, 0], plotting.Field_Options([-3., 3.],
                                                                  [-3., 3.], [30, 30]))
plt.show()

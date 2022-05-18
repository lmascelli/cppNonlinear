from nonlinear import core, analysis
import numpy as np
from typing import List
import matplotlib.pyplot as plt

# LOTKA-VOLTERRA


def f(x: np.ndarray, p: List[float]) -> np.ndarray:
  return np.array([x[0] * (3 - x[0] - 2 * x[1]), x[1] * (2 - x[0] - x[1])])


system = core.SystemDescriptor.BasicSystem(f, core.EQUATION)
window_range = [[-3., 3.], [-3., 3.]]
x0 = np.array([])

manager = analysis.Analysis(system, [], window_range[0], window_range[1],
                            [100, 100])
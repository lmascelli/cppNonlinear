import numpy as np
from typing import List
from nonlinear import core, analysis, gui
from nonlinear.gui import AnalysisGui
from PyQt5.QtWidgets import QApplication
import sys

# LOTKA-VOLTERRA


def f(x: np.ndarray, p: List[float]) -> np.ndarray:
  return np.array([x[0] * (3 - x[0] - 2 * x[1]), x[1] * (2 - x[0] - x[1])])


system = core.SystemDescriptor.BasicSystem(f, core.EQUATION, 2)

app = QApplication(sys.argv)
gui = AnalysisGui(system)
app.exec_()
import matplotlib.pyplot as plt
import numpy as np
from typing import List, Optional
from . import core, pynl_bind


class VectorFieldOptions:
  width: float = 0.001
  scale: Optional[float] = None
  pivot: str = "middle"


def vector_field(
    vf: np.ndarray,
    xrange: List[float],
    yrange: List[float],
    points: List[int],
    axes: Optional[plt.Axes],
    options: VectorFieldOptions = VectorFieldOptions()
) -> None:
  x = np.linspace(xrange[0], xrange[1], points[0])
  y = np.linspace(yrange[0], yrange[1], points[1])

  for i in range(0, points[0]):
    for j in range(0, points[1]):
      if axes:
        axes.quiver(
            x[i],
            y[j],
            vf[0, j, i],
            vf[1, j, i],
            width=0.001,
            scale=None,
            pivot='middle')
      else:
        plt.quiver(
            x[i],
            y[j],
            vf[0, j, i],
            vf[1, j, i],
            width=0.001,
            scale=None,
            pivot='middle')

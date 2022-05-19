import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib import widgets
import numpy as np
from typing import List, Optional, Union
from . import core, pynl_bind, plotting

###############################################################################
#
#                               Analysis Manager
#
###############################################################################


class Analysis:

  def __init__(
      self,
      system: Union[core.SystemDescriptor, pynl_bind.CompiledSystemDescriptor],
  ):

    # system properties
    self.system = system
    self.params: List[float] = []
    self.x0 = np.zeros(system.system_size)

    # vector field properties
    self.xrange: List[List[float]] = []
    self.sampling_points: List[int] = []
    for _ in range(system.system_size):
      self.xrange.append([-1, 1])
      self.sampling_points.append(10)

  ################## Utility ###################

  def traiectory(self, points=10000, step=1e-3) -> Optional[np.ndarray]:
    """
        Compute a traiectory choosing the method based on compiled flag
        """
    if isinstance(self.system, pynl_bind.CompiledSystemDescriptor):
      return pynl_bind.traiectory(self.system, self.x0, self.params, points,
                                  step)
    elif isinstance(self.system, core.SystemDescriptor):
      return core.traiectory(
          self.system,
          np.array(self.x0),
          self.params,  # type: ignore
          points,
          step).T
    else:
      return None

  def compute_vf(self) -> Optional[np.ndarray]:
    if self.system.system_size == 2:
      if isinstance(self.system, pynl_bind.CompiledSystemDescriptor):
        vf = pynl_bind.vector_field_2d(self.system, self.xrange[0],
                                       self.xrange[1], self.sampling_points,
                                       self.params)
      elif isinstance(self.system, core.SystemDescriptor):
        vf = core.vector_field(self.system, self.params, self.xrange[0],
                               self.xrange[1], self.sampling_points)
      else:
        raise Exception("System Type ERROR")

      return vf
    else:
      return None
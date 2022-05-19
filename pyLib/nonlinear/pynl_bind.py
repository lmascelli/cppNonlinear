from typing import List
import system_test
import pynl
import numpy as np
import matplotlib.pyplot as plt


def test(dim: int) -> np.ndarray:
  print('precall')
  ret = pynl.test(dim)
  print('postcall')
  return ret


def traiectory(system: object, x0: np.ndarray, params: List[float],
               n_points: int, step: float) -> np.ndarray:
  return pynl.traiectory(system, x0, params, n_points, step)


def vector_field_2d(system: object, xrange: List[float], yrange: List[float],
                    sampling_points: List[int],
                    params: List[float]) -> np.ndarray:
  return pynl.vector_field_2d(system, xrange[0], xrange[1], yrange[0],
                              yrange[1], sampling_points[0], sampling_points[1],
                              params)


class CompiledSystemDescriptor:

  def __init__(self):
    self.system_size = system_test.GetSystemDescriptor().get_system_size()


def GetSystemDescriptor() -> CompiledSystemDescriptor:
  return CompiledSystemDescriptor()

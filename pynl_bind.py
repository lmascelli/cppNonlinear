from typing import List
import pynl


def traiectory(t0: float, x0: List[float], params: List[float],
               n_points: int, step: float) -> List[List[float]]:
    return pynl.traiectory(t0, x0, params, n_points, step)

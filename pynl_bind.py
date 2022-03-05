from typing import List
import pynl


def traiectory(system: object, t0: float, x0: List[float], params: List[float],
               n_points: int, step: float) -> List[List[float]]:
    return pynl.traiectory(system, t0, x0, params, n_points, step)

from typing import List
import pynl


def traiectory(system: object, t0: float, x0: List[float], params: List[float],
               n_points: int, step: float) -> List[List[float]]:
    return pynl.traiectory(system, t0, x0, params, n_points, step)


def vector_field_2d(system: object, xmin: float, xmax: float, ymin: float,
                    ymax: float, x_points: int, y_points: int, 
                    params: List[float]) -> List[List[List[float]]]:
    return pynl.vector_field_2d(system, xmin, xmax, ymin, ymax, x_points,
                                y_points, params) 

import numpy as np

I: float
alfa: float
gamma: float
beta: float


def system(x: np.ndarray) -> np.ndarray:
    return np.array([
        x[0] - np.power(x[0], 3) - x[1] + I,
        alfa*(x[0] + gamma - beta*x[1])
    ])

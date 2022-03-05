from typing import List
import matplotlib.pyplot as plt
import pynl_bind as nl


def plot_transient(data: List[List[float]]) -> None:
    """
    TODO da aggiungere axis label e title
    """
    plt.subplot(3, 1, 1)
    plt.plot(data[0], data[1])
    plt.subplot(3, 1, 2)
    plt.plot(data[0], data[2])
    plt.subplot(3, 1, 3)
    plt.plot(data[1], data[2])
    plt.show()


t = nl.traiectory(0, [0, 0], [0.5, 0.5], 100000, 1e-3)
print(t)
# if t:
#     plot_transient(t)

from typing import List
import matplotlib.pyplot as plt
import pynl_bind as nl
import system_test


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


system = system_test.GetSystemDescriptor()

nl.vector_field_2d(system, -10, 10, -30, 30, 10, 10, [0.1, 0.2])

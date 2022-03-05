import os
import sys
from typing import List, Tuple

"""
This script is made specifically for the test system so it must load the
computed data and plot:

- transient:
    - manifold cross vs time
    - x vs time
    - t vs time

- shooting:
"""


import matplotlib.pyplot as plt


class Data:
    def __init__(self, t: List[float], x: List[float], y: List[float]):
        self.t = t
        self.x = x
        self.y = y


def manifold(data: Data) -> Tuple[List[float], List[float]]:
    times = []
    values = []
    for i, el in enumerate(data.x):
        if el >= 30:
            times.append(data.t[i])
            values.append(el)
    return (times, values)


def load_data(filepath="data.csv") -> Data:
    """
    It is assumpted that data are located in the same folder of this scriot.
    If not specify the filepath of data.csv
    """
    t = []
    x = []
    y = []
    with open(filepath, 'r') as f:
        line = f.readline()
        line = line.split(',')
        for el in line:
            t.append(float(el))
        line = f.readline()
        line = line.split(',')
        for el in line:
            x.append(float(el))
        line = f.readline()
        line = line.split(',')
        for el in line:
            y.append(float(el))
        return Data(t, x, y)


def plot_transient(data: Data) -> None:
    """
    TODO da aggiungere axis label e title
    """
    plt.subplot(3, 1, 1)
    plt.plot(data.t, data.x)
    plt.plot(manifold(data)[0], manifold(data)[1], 'bo')
    plt.subplot(3, 1, 2)
    plt.plot(data.t, data.y)
    plt.subplot(3, 1, 3)
    plt.plot(data.x, data.y)
    plt.show()


if __name__ == '__main__':
    args = sys.argv[1:]
    for command in args:
        if command == 'plot':
            data = load_data()
            plot_transient(data)
        elif command == 'build':
            os.system('cd build && cmake --build . && cd ..')
        elif command == 'run':
            os.system('cd build && test.exe')
        else:
            print(f'option {command} not valid')

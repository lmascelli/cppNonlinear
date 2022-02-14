import os
import sys
import importlib
from types import FunctionType, ModuleType
from typing import Tuple


def check_system_requirements() -> bool:
    try:
        importlib.import_module('matplotlib')
        import matplotlib.pyplot as plt
        return True
    except ModuleNotFoundError as e:
        print('''
ERROR: module matplotlib not found!
You can solve installing it with:
--------------------------

pip install matplotlib

--------------------------
''')


def plot(filename=None):
    import matplotlib.pyplot as plt

    # TODO on production build refactor the plot path
    if not filename:
        filename = 'temp.csv'
    with open(filename) as f:
        x = []
        y = []
        line = f.readline()
        numbers = line.split(',')
        for i, n in enumerate(numbers):
            x.append(float(n))
        line = f.readline()
        numbers = line.split(',')
        for i, n in enumerate(numbers):
            y.append(float(n))

        plt.plot(x, y)
        plt.show()


if __name__ == '__main__':
    if check_system_requirements():
        # check command arguments
        args = sys.argv
        nargs = len(args)

        if nargs == 1:
            plot()
        if nargs == 2:
            plot(args[1])

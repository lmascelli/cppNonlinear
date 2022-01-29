import matplotlib.pyplot as plt
import os

if __name__ == '__main__':
    filename = 'build/data.csv' if os.name == 'posix' else 'build/Debug/data.csv'
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

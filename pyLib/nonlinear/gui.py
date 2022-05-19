import time
from types import NoneType
from typing import List, Union

import numpy as np
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qtagg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.backends.qt_compat import QtWidgets as qw
from matplotlib.figure import Figure

from . import analysis, core, pynl_bind, plotting


class RangeDialog(qw.QDialog):

    

    def __init__(self, parent: qw.QMainWindow, xrange: List[List[float]]) -> None:
        super().__init__(parent)

        table = qw.QTableWidget(self)
        table.setColumnCount(3)
        table.setRowCount(len(xrange))
        table.setHorizontalHeaderLabels(("", "MIN", "MAX"))

        for i, el in enumerate(xrange):
            table.setItem(i, 0, qw.QTableWidgetItem(str(i)))
            table.setItem(i, 1, qw.QTableWidgetItem(str(el[0])))
            table.setItem(i, 2, qw.QTableWidgetItem(str(el[1])))

        cancel = qw.QPushButton("Cancel", self)
        apply = qw.QPushButton("Apply", self)

        layout = qw.QVBoxLayout(self)
        layout.addWidget(table)
        layout.addWidget(cancel)
        layout.addWidget(apply)
        self.setLayout(layout)
        self.setGeometry(parent.x()+30, parent.y()+30, 350, 300)
        self.setWindowTitle("Adjust Plot Ranges")
        self.show()


class AnalysisGui(qw.QMainWindow):
    def __init__(
        self, system: Union[core.SystemDescriptor, pynl_bind.CompiledSystemDescriptor]
    ) -> None:
        super().__init__()

        self.analysis = analysis.Analysis(system)
        self.setup_gui()
        self.show()

    #   CODE FOR DINAMIC PLOT

    #   dynamic_canvas = FigureCanvas(Figure(figsize=(5, 3)))  # type: ignore
    #   layout.addWidget(dynamic_canvas)
    #   layout.addWidget(NavigationToolbar(dynamic_canvas, self))

    #   self._dynamic_ax = dynamic_canvas.figure.subplots()
    #   t = np.linspace(0, 10, 101)
    #   # Set up a Line2D.
    #   self._line, = self._dynamic_ax.plot(t, np.sin(t + time.time()))
    #   self._timer = dynamic_canvas.new_timer(50)
    #   self._timer.add_callback(self._update_canvas)
    #   self._timer.start()

    # def _update_canvas(self):
    #   t = np.linspace(0, 10, 101)
    #   # Shift the sinusoid as a function of time.
    #   self._line.set_data(t, np.sin(t + time.time()))
    #   self._line.figure.canvas.draw()

    def quit(self):
        qw.QApplication.quit()

    def vector_field(self):
        vf = self.analysis.compute_vf()
        if isinstance(vf, np.ndarray):
            plotting.vector_field(
                vf,
                self.analysis.xrange[0],
                self.analysis.xrange[1],
                self.analysis.sampling_points,
                self.ax,
            )
            self.canvas.draw()
        else:
            qw.QErrorMessage().showMessage(
                "ERROR! probably trying to plot a vector field for a non 2D system"
            )

    def settings_range(self):
        RangeDialog(self, self.analysis.xrange)

    def setup_gui(self):

        # MENU
        menu = qw.QMenuBar(self)
        menu_file = qw.QMenu("&File", menu)
        action_close = qw.QAction("Close", self)
        action_close.triggered.connect(self.quit)
        menu_file.addAction(action_close)

        menu_plot = qw.QMenu("&Plot", self)
        action_vf = qw.QAction("&Vector field (2D systems)", self)
        action_vf.triggered.connect(self.vector_field)
        menu_plot.addAction(action_vf)

        menu_settings = qw.QMenu("&Settings", self)
        action_range = qw.QAction("Adjust plot &range", self)
        action_range.triggered.connect(self.settings_range)
        menu_settings.addAction(action_range)

        menu.addMenu(menu_file)
        menu.addMenu(menu_plot)
        menu.addMenu(menu_settings)
        self.setMenuBar(menu)

        # CANVAS
        self._main = qw.QWidget()
        self.setCentralWidget(self._main)
        layout = qw.QVBoxLayout(self._main)

        self.canvas = FigureCanvas(Figure(figsize=(5, 3)))  # type: ignore
        layout.addWidget(NavigationToolbar(self.canvas, self))
        layout.addWidget(self.canvas)

        self.ax = self.canvas.figure.subplots()

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
from typing import List, Union
from . import core, pynl_bind, plotting

###############################################################################
#
#                               Analysis Manager
#
###############################################################################
'''
The analysis of the system follows this steps:
    [X] plot of the vector field of the system varying with parameters
        and plot of the equilibrium points if any with associated jacobian's
        eigenvector and direction (given by eigenvalues)
    [X] computation of the transient and eventually plot of it selecting the
        initial condition in the state space
    [ ]   plot of the eventual limit cycle
'''


class Analysis:

  def __init__(self,
               system: Union[core.SystemDescriptor,
                             pynl_bind.CompiledSystemDescriptor],
               params: List[float],
               xrange: List[float],
               yrange: List[float],
               sampling_points: List[int],
               compiled: bool = False):

    # system properties
    self.params = params
    self.system = system
    self.compiled = compiled
    self.x0 = [0., 0.]

    # vector field properties
    self.xrange = xrange
    self.yrange = yrange
    self.sampling_points = sampling_points

    # plot window properties
    self.fig, self.ax = plt.subplots()
    self.fig.canvas.mpl_connect('key_press_event', self.on_key_press)
    self.fig.canvas.mpl_connect('button_press_event', self.on_mouse_press)
    self.recompute = True
    self.traiectoryMode = False

    self.plot_vf()

  ################## Utility ###################

  def traiectory(self, points=10000, step=1e-3) -> np.ndarray:
    """
        Compute a traiectory choosing the method based on compiled flag
        """
    t: np.ndarray
    if isinstance(self.system, pynl_bind.CompiledSystemDescriptor):
      pass
      t = np.array([])
      # t = pynl_bind.traiectory(self.system, self.x0, self.params,
      #                         points, step)
    elif isinstance(self.system, core.SystemDescriptor):
      t = core.traiectory(
          self.system,
          np.array(self.x0),
          self.params,  # type: ignore
          points,
          step).T
    else:
      raise Exception("System type ERROR")
    return t

  def get_pivot(self, eig: float) -> str:
    return 'tip' if eig < 0 else 'tail'

  ############### Event handling ##################

  def plot_connect_event(self):
    '''
        Restore the callbacks of the plot window after a redraw
        '''
    self.ax.callbacks.connect('xlim_changed', self.on_xaxes_change)
    self.ax.callbacks.connect('ylim_changed', self.on_yaxes_change)

  def on_key_press(self, event):
    if event.key == 'right' and self.params[0] < 1:
      print('Increasing alpha')
      self.params[0] += 0.1
      self.plot_vf()
    if event.key == 'left' and self.params[0] > 0:
      print('Decreasing alpha')
      self.params[0] -= 0.1
      self.plot_vf()
    if event.key == 'up' and self.params[1] < 1:
      print('Increasing betha')
      self.params[1] += 0.1
      self.plot_vf()
    if event.key == 'down' and self.params[1] > 0:
      print('Decreasing betha')
      self.params[1] -= 0.1
      self.plot_vf()
    if event.key == 'a':
      self.recompute = not self.recompute
      print(f'Recumpute: {"ON" if self.recompute else "OFF"}')
    if event.key == 't':
      self.traiectoryMode = not self.traiectoryMode
      print(f'Trayectory mode: {"ON" if self.traiectoryMode else "OFF"}')

  def on_mouse_press(self, event):
    print(event.xdata, event.ydata)
    self.x0 = [event.xdata, event.ydata]
    if self.traiectoryMode:
      self.animate_traiectory()

  def on_xaxes_change(self, event):
    if self.recompute == True:
      self.xrange = event.get_xlim()
      self.plot_vf()

  def on_yaxes_change(self, event):
    if self.recompute == True:
      self.yrange = event.get_ylim()
      self.plot_vf()

  ################# Plotting section ################

  def plot_vf(self):
    if type(self.system) == pynl_bind.CompiledSystemDescriptor:
      vf = pynl_bind.vector_field_2d(self.system, self.xrange, self.yrange,
                                     self.sampling_points, self.params)
    elif type(self.system) == core.SystemDescriptor:
      vf = core.vector_field(
          self.system,  # type: ignore
          self.params,
          self.xrange,
          self.yrange,
          self.sampling_points)
    else:
      raise Exception("System Type ERROR")

    self.ax.clear()
    plotting.vector_field(vf, self.xrange, self.yrange, self.sampling_points,
                          self.ax)
    self.fig.canvas.draw()

  def animate_traiectory(self):
    """
        Plot a traiectory starting from point clicked in the axes
        """
    t = self.traiectory().T
    nframes = int(len(t[0]) / 100)
    line, = self.ax.plot([self.x0[0]], [self.x0[1]], lw=3)

    def func(frame: int):
      xdata = t[0][0:frame * 100]
      ydata = t[1][0:frame * 100]
      line.set_data(xdata, ydata)
      # self.fig.canvas.draw()
      return line,

    self.anim = FuncAnimation(
        self.fig, func, nframes, blit=True, interval=1, repeat=False)

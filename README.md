# cppNonlinear

## This is an attempt of analysing a non linear system with raw C++ and as less as possible external libraries. Libraries and tools that will be used are:
- <b>Armadillo</b>: A linear algebra library with a syntax that try to resemble to the Matlab one
- <b>Matplotlib</b>: A python toolkit for plotting data; i've choosen a tool for an other language because i've not found any good alternative that works with C++ directly
- <b>Python</b>: A python script for reading and plot results of analysis; the choise of python over matlab comes from the fact i'm more used to work with it

## There is a trade-off between generalization of analysis and the need to get the problem solved in the abstraction level of this work. At this point there is:
- A nonlinear part reseved to general functions used in analysis like:
  - An ODE integrating function with <i>Newton</i> and <i>Runge-Kutta 3rd order</i> algorithms; this point is not so trivial because the system may not be ruled by a single equation (and in this case it is not!).<br>
  This is achieved ("taking inspiration" from matlab ode solver) by passing an event function that check if the traiectory intersect some manifold and returnin the associated label; it's responsability of the calling function
  to change the corrensponding function for the next step.

## Roadmap:
  - [X] Structure an analysis class with all data needed for a point in parameter space
  - [ ] Store and plot a traiectory
  - [X] Compute the transient
  - [ ] Compute the period (T) of a periodic traiectory using the shooting method
  - [ ] Compute the monodomy matrix
  - [ ] Floquet multiplyers
  - [ ] Fill the parameter space

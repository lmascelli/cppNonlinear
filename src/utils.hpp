#pragma once

#include <armadillo>
#include <iostream>
using std::endl;

class Log
{
public:
  static std::ostream &Print();
};

class Plotting
{
public:
  static void PlotMat(arma::mat &m);
};

class Storage
{
};
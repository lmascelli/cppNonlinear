#pragma once

#include <armadillo>
#include <iostream>
using std::endl;

#ifndef VARIABLE_NAME
#define VARIABLE_NAME(V) #V
#endif

#ifndef MAT_SHAPE
#define MAT_SHAPE(m) Log::print_shape(m, VARIABLE_NAME(m));
#endif

class SystemDescriptor;

class Log
{
public:
  static std::ostream &Print();
  static void print_shape(arma::mat &m, std::string name);
};

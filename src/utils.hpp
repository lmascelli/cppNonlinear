#pragma once

#include <armadillo>
#include <iostream>
using std::endl;

class SystemDescriptor;

class Log
{
public:
  static std::ostream &Print();
};
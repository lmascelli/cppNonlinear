#include "src/analysis.hpp"
#include "src/nonlinear.hpp"

mat test_system(double t, mat x, mat param)
{
  const double a = 0.04;
  const double b = 5;
  const double c = 150;
  mat ret{
      a * x(0) + b * x(0) + c - x(1),
      param(0) * (param(1) * x(0) - x(1))};
  return ret.t();
}

mat test_mapping(double t, mat x, mat param)
{
  const double d = 1;
  const double f = 1;
  mat ret{d, x(1) + f};
  return ret.t();
}

uint eventfun(double t, mat x)
{
  const double xTH = 30;
  uint ret = x(0) >= xTH ? 1 : 0;
  return ret;
}

int main(int argc, char const *argv[])
{
  Analysis a([](Analysis &self)
             {
               self.system.push_back(test_system);
               self.system.push_back(test_mapping);
               self.manifold = eventfun;
               self.x0 = {0, 0};
               self.ntransient = 1000;
               self.t0 = 0;
               self.t_transient = 10;
               self.tend = 20;
               self.ndata = 1000;
             });
  return 0;
}

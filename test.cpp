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

mat eventfun(double t, mat x)
{
  const double xTH = 30;
  mat ret = x(0) >= xTH ? mat({0, 1}) : mat({1, 0});
  return ret.t();
}

int main(int argc, char const *argv[])
{
  Analysis a([](Analysis &self) {

  });
  return 0;
}

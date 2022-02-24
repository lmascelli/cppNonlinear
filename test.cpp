#include "src/analysis.hpp"
#include "src/nonlinear.hpp"
#include "src/utils.hpp"

int main(int argc, char const *argv[])
{
  SystemDescriptor system;
  system.AddFunction(
      [](double t, mat x, mat param)
      {
        const double a = 0.04;
        const double b = 5;
        const double c = 150;
        mat ret{a * x(0) * x(0) + b * x(0) + c - x(1),
                param(0) * (param(1) * x(0) - x(1))};
        ret = ret.t();
        return ret; },
      SystemDescriptor::EQUATION);

  system.AddFunction(
      [](double t, mat x, mat param)
      {
        const double d = -55;
        const double f = 4;
        mat ret{d, x(1) + f};
        Log::Print() << "Called" << endl;
        return (mat)ret.t();
      },
      SystemDescriptor::MAP);

  system.manifold = [](double, mat x)
  {
    const double xTH = 30;
    uint ret = x(0) >= xTH ? 1 : 0;
    return ret;
  };

  mat x0 = {0.5, 5};
  mat m = traiectory(system, 0, x0.t(), {0.1, 1}, 100000, 0.001);
  m.save("data.csv", arma::csv_ascii);
  return 0;
}

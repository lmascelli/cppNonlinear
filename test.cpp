#include "src/analysis.hpp"
#include "src/nonlinear.hpp"
#include "src/utils.hpp"

mat test_system(double t, mat x, mat param)
{
  const double a = 0.04;
  const double b = 5;
  const double c = 150;
  mat ret{a * x(0) * x(0) + b * x(0) + c - x(1),
          param(0) * (param(1) * x(0) - x(1))};
  return ret.t();
}

mat test_mapping(double t, mat x, mat param)
{
  const double d = -55;
  const double f = 4;
  mat ret{d, x(1) + f};
  Log::Print() << "Called" << endl;
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

  Analysis a([](Analysis &self)
             {
    self.system = test_system;
    self.map = test_mapping;
    self.manifold = eventfun;
    self.x0 = {0.5, 0.5};
    self.x0 = self.x0.t();
    self.ntransient = 10000;
    self.t0 = 0;
    self.t_transient = 10;
    self.tend = 20;
    self.ndata = 1000; });
  // mat *ret = nullptr;
  // a.transient({0.1, 1}, true, &ret);
  // Log::Print() << ret->n_rows << " X " << ret->n_cols << endl;
  // ret->save("temp.csv", arma::file_type::csv_ascii);

  // Plotting::PlotMat(*ret);

  mat x0 = {0.5, 0.5};
  mat m = traiectory(system, 0, x0.t(), {0.1, 1}, 1000, 0.01);
  Log::Print() << m << endl;
  Plotting::PlotMat(m);
  return 0;
}

#include "src/analysis.hpp"
#include "src/nonlinear.hpp"
#include "src/utils.hpp"

int main(int argc, char const *argv[])
{
  SystemDescriptor system;

  /**
   * @brief main system function
   * dx = a(x^2 + bx+ c - y)
   * dy = α(βx -y)
   */
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

  /**
   * @brief reset rule map
   *  x -> -55
   *  y -> y + 4
   */
  system.AddFunction(
      [](double t, mat x, mat param)
      {
        const double d = -55;
        const double f = 4;
        mat ret{d, x(1) + f};
        return (mat)ret.t();
      },
      SystemDescriptor::MAP);

  /**
   * @brief the manifold of reset
   *
   *  x >= 30
   */
  system.manifold = [](double, mat x)
  {
    const double xTH = 30;
    uint ret = x(0) >= xTH ? 1 : 0;
    return ret;
  };

  /**
   * @brief the gradient of the manifold
   * manifold(x, y) = x - 30
   * gradient -> (1, 0)
   */
  system.manifold_gradient = [](double t, mat x, mat params)
  {
    mat ret{1, 0};
    return ret;
  };

  /**
   * @brief the time derivative is always 0
   *
   */
  system.manifold_time_derivative = [](double t, mat x, mat params)
  {
    mat ret = arma::zeros(1, 1);
    return ret;
  };

  mat x0 = {0.5, 5};
  mat m = traiectory(system, 0, x0.t(), {0.1, 1}, 100000, 0.001);
  constexpr double final_time = 100000 * 0.001;

  mat monodromy = shooting(system, final_time, m.col(m.n_cols - 1), {0.1, 1}, 10.0, 0.001, 10);

  Log::Print() << monodromy << endl;

  m.save("data.csv", arma::csv_ascii);
  return 0;
}
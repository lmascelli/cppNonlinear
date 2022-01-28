#include "analysis.hpp"
#include "nonlinear.hpp"

Analysis::Analysis(void (*init_func)(Analysis &self)) { init_func(*this); }

mat Analysis::transient(mat param, bool saveData, mat *data)
{
  if (saveData)
    *data = mat(x0.n_rows, ntransient);
  double t = t0;
  mat x = x0;
  double step = (tend - t0) / ntransient;
  uint i = 0;
  while (t < tend)
  {
    x = integrate(system, t, x, param, step);
    if (saveData)
    {
      data->col(i++) = x;
    }
  }
  return x;
}

#include "analysis.hpp"
#include "nonlinear.hpp"

Analysis::Analysis(void (*init_func)(Analysis &self))
{
  init_func(*this);
  uint param_number = param_range.n_rows;
  params = mat(param_number, nparams);
  for (uint i = 0; i < param_number; i++)
  {
    params.row(i) = linspace(param_range(i, 0), param_range(i, 1), nparams);
  }
  current_param = params.col(0);
}

mat Analysis::transient(mat param, bool saveData, mat *data)
{
  if (saveData)
    *data = mat(x0.n_rows, ntransient);
  double t = t0;
  mat x = x0;
  double step = (tend - t0) / ntransient;
  uint i = 0;
  event_struct result;
  result.t = t0;
  result.x = x;
  result.event = 0;
  while (t < tend)
  {
    event_struct new_result;
    x = integrate(current, t, x, param, step, 10, manifold, &new_result, "newton");
    if (new_result.event != result.event)
      current = system[new_result.event];
    if (saveData)
      data->col(i++) = x;
  }
  return x;
}

#include "analysis.hpp"
#include "nonlinear.hpp"

Analysis::Analysis(void (*init_func)(Analysis &self))
{
  init_func(*this);
  current = system[manifold(t0, x0)];
}

mat Analysis::transient(mat param, bool saveData, mat **data)
{
  if (saveData)
  {
    *data = new mat(x0.n_rows, ntransient);
    cout << "Saving transient data in " << (*data)->n_rows << " X "
         << (*data)->n_cols << " matrix" << endl;
  }
  double t = t0;
  mat x = x0;
  double step = (t_transient - t0) / (ntransient - 1);
  uint i = 0;
  Event_struct result;
  result.t = t0;
  result.x = *x;
  result->event = manifold(t0, x);
  {
    cout << "Initial conditions = " << x
         << "Initial time = " << t0 << endl
         << "End transient time = " << t_transient << endl
         << "Time step = " << step << endl;
  }
  cout << "Integrating..." << endl;
  while (t <= t_transient)
  {
    event_struct new_result;
    new_result.event = manifold(t, x);
    x = integrate(current, t, x, param, step, 10, manifold, &new_result, "newton");

    if (new_result.event != result.event)
    {
      cout << new_result.event << " at t: " << new_result.t << ", x: "
           << new_result.x << endl;
      current = system[new_result.event];
    }
    if (saveData)
      (*data)->col(i++) = x;
    // cout << i << " iteration -> t: " << t << ", x: " << (*data)->col(i - 1) << endl;
    t += step;
  }
  return x;
}

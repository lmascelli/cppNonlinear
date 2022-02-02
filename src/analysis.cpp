#include "analysis.hpp"
#include "nonlinear.hpp"

Analysis::Analysis(void (*init_func)(Analysis &self)) {
  init_func(*this);
  // TODO also remove this if the program still works
  // current_state = manifold(t0, x0);
}

mat Analysis::transient(mat param, bool saveData, mat **data) {
  if (saveData) {
    *data = new mat(x0.n_rows, ntransient);
    cout << "Saving transient data in " << (*data)->n_rows << " X "
         << (*data)->n_cols << " matrix" << endl;
  }
  double t = t0;
  mat x = x0;
  double step = (t_transient - t0) / (ntransient - 1);
  uint i = 0;
  {
    cout << "Initial conditions = " << x << "Initial time = " << t0 << endl
         << "End transient time = " << t_transient << endl
         << "Time step = " << step << endl;
  }
  cout << "Integrating..." << endl;
  while (t <= t_transient) {
    event_struct new_result;
    new_result.event = manifold(t, x);
    x = integrate(system, t, x, param, step, 10, manifold, &new_result, "rk3");
    if (new_result.event == 1) {
      // cout << "NEW AREA MANIFOLD: " << new_result.event << " at t: " <<
      // new_result.t << ", x: "
      //      << new_result.x << endl;
      // cout << new_result.event << endl;
      x = map(new_result.t, new_result.x, param);
    } else {
      // cout << "SAME AREA MANIFOLD: " << new_result.x << endl;
    }
    if (saveData)
      (*data)->col(i++) = x;
    // cout << i << " iteration -> t: " << t << ", x: " << (*data)->col(i - 1)
    // << endl;
    t += step;
  }
  return x;
}

mat Analysis::shooting(mat param, bool saveData, mat **data) {
  *data = new mat(current_x.n_rows, ndata);
  mat x = current_x;
  double t = t_transient;
  uint n = 2;
  mat Iin = eye(n, n);

  bool running = true;
  for (uint i = 0; i < niter && running; i++) {
    double step = (tend - t_transient) / ndata;
    while (t < tend) {
      event_struct result;
      x = integrate(system, t, x, param, step, 10, manifold, &result, "rk3");
    }
  }
  return {};
}

#include "analysis.hpp"
#include "nonlinear.hpp"

Analysis::Analysis(void (*init_func)(Analysis &self)) {
  init_func(*this);
  params = linspace(param_ranges(0), param_ranges(1), nparam);
  x_data = zeros(nparam, ndata);
  if (type != "discrete" && type != "continue")
    cout << "analysis type <" << type << "> not valid. Analysis may not work"
         << endl;
}

mat Analysis::transient(bool saveData) {
  mat ret;
  double t = t0;
  double step = (tend - t0) / ntransient;
  while (t < tend) {
  }
  return ret;
}

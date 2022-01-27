#include "nonlinear.hpp"

mat integrate(function<mat(double, mat, void *)> f, double t0, mat x0,
              void *args, double h, uint n_steps, string method) {
  mat ret = x0;

  if (method == "newton")
    for (uint i = 1; i < n_steps; i++) {
      x0 += h * f(t0, x0, args);
      t0 += h;
      ret = x0;
    }
  if (method == "rk3")
    for (uint i = 1; i < n_steps; i++) {
      mat k0 = f(t0, x0, args) * h;
      mat k1 = f(t0 + 0.5 * h, x0 + 0.5 * k0, args) * h;
      mat k2 = f(t0 + 0.5 * h, x0 + 0.5 * k1, args) * h;
      mat k3 = f(t0 + h, x0 + k2, args) * h;
      ret = 1. / 6. * (k0 + 2 * k1 + 2 * k2 + k3);
    }
  return ret;
}

mat transient_continue(function<mat(double, mat, void *)> system, double t,
                       mat x, void *args) {
  return {};
}

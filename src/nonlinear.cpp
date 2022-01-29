#include "nonlinear.hpp"

mat integrate(mat (*f)(double, mat, mat), double t0, mat x0, mat args,
              double h, uint n_steps, uint (*event)(double, mat),
              event_struct *result, string method)
{
  double current_state;
  if (event)
  {
    current_state = event(t0, x0);
  }

  h /= (double)n_steps;

  if (method == "newton")
    for (uint i = 0; i < n_steps; i++)
    {
      x0 += h * f(t0, x0, args);
      if (event)
      {
        uint new_state = event(t0, x0);
        // cout << "integration step progress. cycle " << i
        //      << " value " << x0 << " system " << new_state << endl;
        if (new_state != current_state)
        {
          result->event = event(t0, x0);
          result->t = t0;
          result->x = x0;
          return x0;
        }
      }
      t0 += h;
    }
  if (method == "rk3")
    for (uint i = 0; i < n_steps; i++)
    {
      mat k0 = f(t0, x0, args) * h;
      mat k1 = f(t0 + 0.5 * h, x0 + 0.5 * h * k0, args) * h;
      mat k2 = f(t0 + 0.5 * h, x0 + 0.5 * h * k1, args) * h;
      mat k3 = f(t0 + h, x0 + h * k2, args) * h;
      x0 += 1. / 6. * (k0 + 2 * k1 + 2 * k2 + k3);
      if (event)
      {
        uint new_state = event(t0, x0);
        if (new_state != current_state)
        {
          result->event = event(t0, x0);
          result->t = t0;
          result->x = x0;
          return x0;
        }
      }
      t0 += h;
    }
  return x0;
}

#include "nonlinear.hpp"

Event_struct::Event_struct()
{
  x = nullptr;
}

Event_struct::~Event_struct()
{
  if (x)
    delete x;
}

mat integrate(mat (*f)(double, mat, mat), double t0, mat x0, mat args,
              double h, uint n_steps, uint (*event)(double, mat),
              Event_struct *result, string method)
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
        double new_state = event(t0, x0);
        if (new_state != current_state)
        {
          result->event = event(t0, x0);
          result->t = t0;
          result->x = *x0;
          return x0;
        }
      }
      t0 += h;
    }
  if (method == "rk3")
    for (uint i = 0; i < n_steps; i++)
    {
      mat k0 = f(t0, x0, args) * h;
      mat k1 = f(t0 + 0.5 * h, x0 + 0.5 * k0, args) * h;
      mat k2 = f(t0 + 0.5 * h, x0 + 0.5 * k1, args) * h;
      mat k3 = f(t0 + h, x0 + k2, args) * h;
      x0 = 1. / 6. * (k0 + 2 * k1 + 2 * k2 + k3);
      if (event)
      {
        double new_state = event(t0, x0);
        if (new_state != current_state)
        {
          result->event = event(t0, x0);
          result->t = t0;
          result->x = *x0;
          return x0;
        }
      }
      t0 += h;
    }
  return x0;
}

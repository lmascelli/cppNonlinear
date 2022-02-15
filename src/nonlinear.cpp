#include "nonlinear.hpp"
#include "utils.hpp"

/**
 * ----------------------------------------------------------------------------
 * SystemDescriptor METHODS
 * ----------------------------------------------------------------------------
 */

system_func SystemDescriptor::GetFunction(const uint i) const
{
  return F[i];
}

SystemDescriptor::FunctionType SystemDescriptor::GetType(const uint i) const
{
  return Ft[i];
}

void SystemDescriptor::AddFunction(system_func f, SystemDescriptor::FunctionType type, system_func jac)
{
  F.push_back(f);
  Ft.push_back(type);
  FJ.push_back(jac);
}

/**
 * ----------------------------------------------------------------------------
 * FUNCTIONS IMPLEMENTATION
 * ----------------------------------------------------------------------------
 */

mat integrate(system_func f, double t0, mat x0, mat args, double h,
              uint n_steps, event_func event, EventStruct *result,
              std::string method)
{
  uint current_state;
  if (event)
  {
    current_state = event(t0, x0);
  }

  h /= (double)n_steps;

  if (method == "euler")
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

mat traiectory(SystemDescriptor system, double t0, mat x0, mat params,
               uint n_points, double step, std::string method)
{
  const uint return_size = 1 + x0.n_rows;
  mat ret = arma::zeros(return_size, n_points);

  auto setCol = [&ret, return_size](uint pos, double t, mat x) mutable
  {
    ret(0, pos) = t;
    ret(arma::span(1, return_size - 1), pos) = x;
  };

  for (uint i = 0; i < n_points; i++)
  {
    uint current_label = system.manifold(t0, x0);
    EventStruct result = {current_label, t0, x0};
    switch (system.GetType(current_label))
    {
    case SystemDescriptor::EQUATION:
    {
      x0 = integrate(system.GetFunction(current_label), t0, x0, params, step,
                     10, system.manifold, &result, "euler");
      if (result.event != current_label)
      {
        setCol(i, result.t, result.x);
        x0 = system.GetFunction(result.event)(result.t, result.x, params);
        t0 += step;
        break;
      }
      t0 += step;
      setCol(i, t0, x0);
    }
    break;
    case SystemDescriptor::MAP:
    {
      x0 = system.GetFunction(current_label)(t0, x0, params);
      t0 += step;
      setCol(i, t0, x0);
      Log::Print() << "MAP! Cycle: " << i << ", t: " << t0 << ", x: " << x0 << endl;
    }
    break;
    }
  }
  return ret;
}

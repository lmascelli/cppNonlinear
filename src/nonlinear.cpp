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

system_func SystemDescriptor::GetJacobian(const uint i) const
{
  return FJ[i];
}

void SystemDescriptor::AddFunction(system_func f,
                                   SystemDescriptor::FunctionType type,
                                   system_func jac)
{
  F.push_back(f);
  Ft.push_back(type);
  if (jac)
    FJ.push_back(jac);
  else
  {
    Log::Print() << "No analytical jacobian for func " << F.size() - 1
                 << "It will be calculated analytically if required" << endl;
    FJ.push_back(nullptr);
  }
}

/**
 * ----------------------------------------------------------------------------
 * FUNCTIONS IMPLEMENTATION
 * ----------------------------------------------------------------------------
 */

mat integrate(SystemDescriptor &system, double t0, mat x0, mat args,
              double step, EventStruct *event_result, uint sub_steps,
              std::string method)
{
  uint current_state;
  if (system.manifold)
  {
    current_state = system.manifold(t0, x0);
  }

  step /= (double)sub_steps;

  if (method == "euler")
  {
    for (uint i = 0; i < sub_steps; i++)
    {
      x0 += step * system.GetFunction(current_state)(t0, x0, args);
      t0 += step;
      if (system.manifold)
      {
        uint new_state = system.manifold(t0, x0);
        if (new_state != current_state)
        {
          event_result->event = new_state;
          event_result->t = t0;
          event_result->x = x0;
          return x0;
        }
      }
    }
  }
  else if (method == "rk3")
  {
    for (uint i = 0; i < sub_steps; i++)
    {
      auto f = system.GetFunction(current_state);
      mat k0 = f(t0, x0, args) * step;
      mat k1 = f(t0 + 0.5 * step, x0 + 0.5 * step * k0, args) * step;
      mat k2 = f(t0 + 0.5 * step, x0 + 0.5 * step * k1, args) * step;
      mat k3 = f(t0 + step, x0 + step * k2, args) * step;
      x0 += 1. / 6. * (k0 + 2 * k1 + 2 * k2 + k3);
      t0 += step;
      if (system.manifold)
      {
        uint new_state = system.manifold(t0, x0);
        if (new_state != current_state)
        {
          event_result->event = new_state;
          event_result->t = t0;
          event_result->x = x0;
          return x0;
        }
      }
    }
  }
  return x0;
}

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
  else if (method == "rk3")
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

/**
 * TODO check if the function switch the current label in case of manifold cross.
 */
mat traiectory(SystemDescriptor &system, double t0, mat x0, mat params,
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
    uint current_label = 0;
    if (system.manifold)
    {
      current_label = system.manifold(t0, x0);
    }
    EventStruct result = EventStruct{current_label, t0, x0};
    switch (system.GetType(current_label))
    {
    case SystemDescriptor::EQUATION:
    {
      x0 = integrate(system.GetFunction(current_label), t0, x0, params, step,
                     10, system.manifold, &result, "euler");

      if (result.event != current_label)
      {
        setCol(i, result.t, result.x);
        current_label = result.event;
      }
      else
      {
        setCol(i, t0, x0);
      }

      t0 += step;
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

mat jacobian(system_func f, double t, mat x, mat args, double h)
{
  uint dim = x.n_rows;
  mat jac(dim, dim);
  for (uint i = 0; i < dim; i++)
  {
    mat H = arma::zeros(dim, 1);
    H.row(i) += h;
    jac.col(i) = (f(t, x + H, args) - f(t, x - H, args)) / 2 * h;
  }
  return jac;
}

mat transition_matrix(SystemDescriptor &system, double t0, mat x0, mat params,
                      double T, double step, bool save_traiectory,
                      mat *traiectory, std::string method)
{
  const uint dim = x0.n_rows;
  mat m_matrix = arma::eye(dim, dim); // the initial transition matrix is I: x0 = I * x0
  const uint n_step = floor((T - t0) / step);
  if (save_traiectory)
  {
    *traiectory = mat(dim + 1, n_step);
  }
  for (uint i = 0; i < n_step; i++)
  {
    uint current_event = 0;
    EventStruct result{current_event, t0, x0};
    if (system.manifold)
    {
      current_event = system.manifold(t0, x0);
    }
    x0 = integrate(system, t0, x0, params, step, &result);
    t0 += step;
    if (save_traiectory)
    {
      traiectory->operator()(0, i) = t0;
      traiectory->operator()(arma::span(1, dim), i) = x0;
    }
    if (result.event == current_event)
    {
      auto jac = system.GetJacobian(result.event);
      m_matrix *= arma::expmat(
          (jac == nullptr ? jacobian(system.GetFunction(result.event), t0,
                                     x0, params)
                          : jac(t0, x0, params)) *
          step);
    }
    else
    {
      // TODO: compute the saltation matrix
      if (system.GetType(result.event) == SystemDescriptor::EQUATION)
      {
      }
      else if (system.GetType(result.event) == SystemDescriptor::MAP)
      {
      }
    }
  }
  return m_matrix;
}

// TODO consider returning event times

mat shooting(SystemDescriptor &system, double t0, mat x0, mat params, double T,
             double step, uint max_iters, bool save_traiectory = false,
             mat *traiectory = nullptr, std::string method = "euler")
{
  const uint system_size = x0.n_rows;
  mat W = arma::eye(system_size, system_size);
  const uint n_steps = ceil((T - t0) / step);

  if (save_traiectory && !traiectory)
  {
    *traiectory = mat(system_size + 1, n_steps);
  }

  uint current_label = system.manifold(t0, x0);

  return W;
}
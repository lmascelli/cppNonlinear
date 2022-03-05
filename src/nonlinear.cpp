#include "nonlinear.hpp"
#include <cassert>
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
    //    FJ.push_back([f](double t, mat x, mat params) {
    //          mat ret = jacobian(f, t, x, params);
    //          return ret;
    //        });
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
  uint current_state = 0;
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
      x0 += 1. / 6. * (k0 + 2 * k1 + k2 + k3);
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

/**
 * TODO check if the function switch the current label in case of manifold cross.
 */
mat traiectory(SystemDescriptor &system, double t0, mat x0, mat params,
               uint n_points, double step, std::string method)
{
  const uint system_size = x0.n_rows;
  mat ret = arma::zeros(system_size + 1, n_points);

  auto setCol = [&ret, system_size](uint pos, double t, mat x) mutable
  {
    ret(0, pos) = t;
    ret(arma::span(1, system_size), pos) = x;
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
      x0 = integrate(system, t0, x0, params, step, &result, 10, method);

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

mat saltation_matrix(SystemDescriptor &system, double t, mat x_previous,
                     mat x_next, mat params, uint label1, uint label2)
{
  switch (system.GetType(label2))
  {
  case SystemDescriptor::MAP:
  {
    auto jac_fun = system.GetJacobian(label2);
    mat jac;
    if (jac_fun)
      jac = jac_fun(t, x_previous, params);
    else
      jac = jacobian(system.GetFunction(label2), t, x_previous, params);
    mat num = (system.GetFunction(label2)(t, x_next, params) -
               jac * system.GetFunction(label1)(t, x_previous, params));
    mat den = (system.manifold_gradient(t, x_previous, params).t() -
               system.GetFunction(label1)(t, x_previous, params)) *
              system.manifold_time_derivative(t, x_previous, params);
    return jac + num / den * system.manifold_gradient(t, x_previous, params);
  }
  case SystemDescriptor::EQUATION:
  {
    return mat(1, 1);
  }
  default:
    return mat(0, 0);
  }
}

mat transition_matrix(SystemDescriptor &system, double t0, mat x0, mat params,
                      double T, double step, bool save_traiectory,
                      mat *traiectory, std::string method)
{
  const uint system_size = x0.n_rows;
  auto setCol = [traiectory, system_size](uint pos, double t, mat x) mutable
  {
    traiectory->operator()(0, pos) = t;
    traiectory->operator()(arma::span(1, system_size), pos) = x;
  };
  mat m_matrix = arma::eye(system_size, system_size); // initial matrix is I: x0 = I * x0
  const uint n_step = floor(T / step);
  if (save_traiectory)
  {
    *traiectory = mat(system_size + 1, n_step);
  }
  for (uint i = 0; i < n_step; i++)
  {
    uint current_event = 0;
    EventStruct result{current_event, t0, x0};
    current_event = system.manifold(t0, x0);
    switch (system.GetType(current_event))
    {
    case SystemDescriptor::EQUATION:
    {
      x0 = integrate(system, t0, x0, params, step, &result);
      if (current_event == result.event)
      {
        // transation matrix
        {
        }
        if (save_traiectory)
          setCol(i, t0, x0);
      }
      else
      {
        current_event = result.event;
        // saltation matrix
        if (save_traiectory)
          setCol(i, result.t, result.x);
      }
    }
    break;
    case SystemDescriptor::MAP:
    {
      x0 = system.GetFunction(current_event)(t0, x0, params);
      if (save_traiectory)
        setCol(i, t0, x0);
    }
    break;
    }
    t0 += step;
  }
  return m_matrix;
}

// TODO consider returning event times maybe in a row with value 1 each time an
// event occurs.
// TODO discover initial condition for matrix in shooting

mat shooting(SystemDescriptor &system, double t0, const mat x0, mat params,
             double T, double step, uint max_iters, bool save_traiectory,
             mat *traiectory, std::string method)
{
  assert(x0.n_rows == 2 and x0.n_cols == 1);   // Check x0 is a column vector
  const uint system_size = x0.n_rows;          // System size
  mat W = arma::eye(system_size, system_size); // Initial transition matrix
  const uint n_steps = ceil(T / step);         // number of integration steps

  if (save_traiectory && traiectory)
  {
    *traiectory = mat(system_size + 1, n_steps);
  }

  uint current_label = system.manifold(t0, x0);

  for (unsigned int k = 0; k < max_iters; k++)
  {
  }

  return W;
}

std::vector<mat> vector_field_2d(SystemDescriptor &system, double xmin,
                                 double xmax, double ymin, double ymax,
                                 uint x_points, uint y_points, mat params)
{
  std::vector<mat> ret;
  ret.resize(x_points * y_points);
  const double dx = (xmax - xmin) / x_points;
  const double dy = (ymax - ymin) / y_points;

  for (uint i = 0; i < x_points; i++)
    for (int j = 0; j < y_points; j++)
    {
      mat x = {xmin + dx * i, ymin + dx * i};
      uint label = system.manifold(0, x.t());
      ret[j * x_points + i] = x;
    }

  return ret;
}
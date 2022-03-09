#include "nonlinear.hpp"
#include <assert.h>
#include <stdlib.h>
/**
 * ----------------------------------------------------------------------------
 * SystemDescriptor METHODS
 * ----------------------------------------------------------------------------
 */

SystemDescriptor::SystemDescriptor(uint n_func)
{
  current_insered_function = 0;
  this->n_func = n_func;
  this->F = (System_func*)malloc(n_func*sizeof(System_func));
  this->Ft = (FunctionType*)malloc(n_func*sizeof(FunctionType));
  this->Fj = (System_func*)malloc(n_func*sizeof(System_func));
}

SystemDescriptor::~SystemDescriptor(){
  free(Fj);
  free(Ft);
  free(F);
}

System_func SystemDescriptor::GetFunction(const uint i) const
{
  assert(i >= 0 && i < this->current_insered_function);
  return F[i];
}

SystemDescriptor::FunctionType SystemDescriptor::GetType(const uint i) const
{
  assert(i >= 0 && i < this->current_insered_function);
  return Ft[i];
}

System_func SystemDescriptor::GetJacobian(const uint i) const
{
  assert(i >= 0 && i < this->current_insered_function);
  return FJ[i];
}

void SystemDescriptor::AddFunction(System_func f,
                                   SystemDescriptor::FunctionType type,
                                   System_func jac)
{
  F[current_insered_function] = f;
  Ft[current_insered_function] = type;
  if (jac)
    Fj[current_insered_function] = jac;
  else
  {
    Fj[current_insered_function] = NULL;
  }
  current_insered_function++;
}

/**
 * ----------------------------------------------------------------------------
 * FUNCTIONS IMPLEMENTATION
 * ----------------------------------------------------------------------------
 */

IntegrationResult integrate(SystemDescriptor &system, mat x0, mat params,
              double step, EventStruct *event_result, uint sub_steps,
              std::string method)
{
  uint current_state = 0;
  if (system.manifold)
  {
    current_state = system.manifold(x0);
  }

  step /= (double)sub_steps;

  if (method == "euler")
  {
    for (uint i = 0; i < sub_steps; i++)
    {
      x0 += step * system.GetFunction(current_state)(x0, params);
      if (system.manifold)
      {
        uint new_state = system.manifold(x0);
        if (new_state != current_state)
        {
          event_result->event = new_state;
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
      mat k0 = f(x0, params) * step;
      mat k1 = f(x0 + 0.5 * step * k0, params) * step;
      mat k2 = f(x0 + 0.5 * step * k1, params) * step;
      mat k3 = f(x0 + step * k2, params) * step;
      x0 += 1. / 6. * (k0 + 2 * k1 + k2 + k3);
      if (system.manifold)
      {
        uint new_state = system.manifold(x0);
        if (new_state != current_state)
        {
          event_result->event = new_state;
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
mat traiectory(SystemDescriptor &system, mat x0, mat params,
               uint n_points, double step, std::string method)
{
  const uint system_size = x0.n_rows;
  mat ret = arma::zeros(system_size, n_points);

  auto setCol = [&ret](uint pos, mat x) mutable
  {
    ret.col(pos) = x;
  };
  for (uint i = 0; i < n_points; i++)
  {
    uint current_label = 0;
    if (system.manifold)
    {
      current_label = system.manifold(x0);
    }
    EventStruct result = EventStruct{current_label, x0};
    switch (system.GetType(current_label))
    {
    case SystemDescriptor::EQUATION:
    {
      x0 = integrate(system, x0, params, step, &result, 10, method);

      if (result.event != current_label)
      {
        setCol(i, result.x);
        current_label = result.event;
      }
      else
      {
        setCol(i, x0);
      }
    }
    break;
    case SystemDescriptor::MAP:
    {
      x0 = system.GetFunction(current_label)(x0, params);
      setCol(i, x0);
    }
    break;
    }
  }
  return ret;
}

mat jacobian(System_func f, mat x, mat params, double h)
{
  uint dim = x.n_rows;
  mat jac(dim, dim);
  for (uint i = 0; i < dim; i++)
  {
    mat H = arma::zeros(dim, 1);
    H.row(i) += h;
    jac.col(i) = (f(x + H, params) - f(x - H, params)) / 2 * h;
  }
  return jac;
}

mat saltation_matrix(SystemDescriptor &system, mat x_previous,
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
      jac = jacobian(system.GetFunction(label2),x_previous, params);
    mat num = (system.GetFunction(label2)(x_next, params) -
               jac * system.GetFunction(label1)(x_previous, params));
    mat den = (system.manifold_gradient(t, x_previous, params).t() -
               system.GetFunction(label1)(x_previous, params)) *
              system.manifold_time_derivative(x_previous, params);
    return jac + num / den * system.manifold_gradient(x_previous, params);
  }
  case SystemDescriptor::EQUATION:
  {
    return mat(1, 1);
  }
  default:
    return mat(0, 0);
  }
}

mat transition_matrix(SystemDescriptor &system, mat x0, mat params,
                      double T, double step, bool save_traiectory,
                      mat *traiectory, std::string method)
{
  const uint system_size = x0.n_rows;
  auto setCol = [traiectory](uint pos, mat x) mutable
  {
    traiectory->col(pos) = x;
  };
  mat m_matrix = arma::eye(system_size, system_size); // initial matrix is I: x0 = I * x0
  const uint n_step = (uint)floor(T / step);
  if (save_traiectory)
  {
    *traiectory = mat(system_size + 1, n_step);
  }
  for (uint i = 0; i < n_step; i++)
  {
    uint current_event = 0;
    EventStruct result{current_event, x0};
    current_event = system.manifold(x0);
    switch (system.GetType(current_event))
    {
    case SystemDescriptor::EQUATION:
    {
      x0 = integrate(system,x0, params, step, &result);
      if (current_event == result.event)
      {
        // transation matrix
        {
        }
        if (save_traiectory)
          setCol(i, x0);
      }
      else
      {
        current_event = result.event;
        // saltation matrix
        if (save_traiectory)
          setCol(i, result.x);
      }
    }
    break;
    case SystemDescriptor::MAP:
    {
      x0 = system.GetFunction(current_event)(x0, params);
      if (save_traiectory)
        setCol(i, x0);
    }
    break;
    }
  }
  return m_matrix;
}

// TODO consider returning event times maybe in a row with value 1 each time an
// event occurs.
// TODO discover initial condition for matrix in shooting

mat shooting(SystemDescriptor &system, const mat x0, mat params,
             double T, double step, uint max_iters, bool save_traiectory,
             mat *traiectory, std::string method)
{
  assert(x0.n_rows == 2 and x0.n_cols == 1);   // Check x0 is a column vector
  const uint system_size = x0.n_rows;          // System size
  mat W = arma::eye(system_size, system_size); // Initial transition matrix
  const uint n_steps = (uint)ceil(T / step);   // number of integration steps

  if (save_traiectory && traiectory)
  {
    *traiectory = mat(system_size, n_steps);
  }

  uint current_label = system.manifold(x0);

  for (unsigned int k = 0; k < max_iters; k++)
  {
  }

  return W;
}

std::vector<mat> vector_field_2d(SystemDescriptor &system, double xmin,
                                 double xmax, double ymin, double ymax,
                                 uint x_points, uint y_points, mat params)
{
  std::vector<arma::Mat<double>> ret;
  ret.resize(x_points * y_points);

  const double dx = (xmax - xmin) / (double)x_points;
  const double dy = (ymax - ymin) / (double)y_points;

  for (double i = 0; i < x_points; i++)
    for (double j = 0; j < y_points; j++)
    {
      mat x = {xmin + dx * i, ymin + dx * i};
      uint label = system.manifold(0, x.t());
      switch (system.GetType(label))
      {
      case SystemDescriptor::EQUATION:
        ret[j * x_points + i] = system.GetFunction(label)(0, x, params);
        break;
      case SystemDescriptor::MAP:
        ret[j * x_points + i] = arma::zeros(2, 1);
        break;
      }
    }

  return ret;
}

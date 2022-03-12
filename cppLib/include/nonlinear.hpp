#pragma once

#include "global.hpp"
#include <armadillo>
using arma::mat;

/*
 * ----------------------------------------------------------------------------
 * TYPES DECLARATION
 * ----------------------------------------------------------------------------
 */
using uint = unsigned int;

/**
 * @brief Type representing a function that describe a system
 * @param t explicit time
 * @param x COLUMN array with point where to evaluate the system
 * @param params COLUMN array containg the paramaters value of the system
 * @return the value of the system at time T, coords X, parameters PARAMS
 */
using System_func = mat (*)(mat, mat);
/**
 * @brief This is a function evaluating the region of space where different
 *        functions rule the system.
 * @param t time
 * @param x coords
 * @return a label rapresenting the region where the system is.
 */
using Event_func = uint (*)(mat);

/*
 * ----------------------------------------------------------------------------
 * STRUCTURES AND CLASSED
 * ----------------------------------------------------------------------------
 */

/**
 * This class contains the ruling equations of the system and the manifold
 * rules, if any, to switch between them.
 */
class SystemDescriptor
{
public:
  /**
   * The funtions ruling the system may be equation or map.
   * Possible values: {EQUATION, MAP}
   */
  enum FunctionType
  {
    EQUATION,
    MAP
  };

  /**
   * @brief SystemDescriptor constructor function
   * @param n_func the number of functions of the system, be them equations or
   *        map
   */
  SystemDescriptor(uint n_func);
  ~SystemDescriptor();

  /**
   * @brief Insert a function in the system
   * @param f the function to insert
   * @param t the type of the function, default EQUATION
   * @param jac the jacobian of f, if none it will be computed numerically
   */
  void AddFunction(System_func f, FunctionType t = FunctionType::EQUATION,
                   System_func jac = nullptr);
  /**
   * @brief Returns the ith function of the system
   * @param i the index of the function
   * @return The system function associated to that index
   */
  System_func GetFunction(const uint i) const;

  /**
   * @brief Get the Jacobian function if any, otherwise it return a numerically
   *        computation of it.
   *
   * @param i the index of the function
   * @return Analitically if provided jacobian, otherwise numerical.
   */
  System_func GetJacobian(const uint i) const;

  /**
   * @brief Ge
   *
   * @param i the index of the function
   * @return FunctionType The FunctionType of the ith function;
   */
  FunctionType GetType(const uint i) const;

  Event_func manifold; // the function that given a point in the state-time
                       // parameter returns the index of the corresponding
                       // equation int vector F.

  System_func manifold_gradient;
  System_func manifold_time_derivative;

private:
  uint current_insered_function;
  uint n_func;      // Number of system functions
  System_func *F;   // System functions
  FunctionType *Ft; // System functions types
  System_func *Fj;  // System functions jacobian
};

/**
 * @brief The result of an integration step
 *
 * (x, event)
 * x:     the next s
 * event: the flag of the new field if some, -1 otherwise
 */

using IntegrationResult = struct
{
  mat x;
  uint event;
};

/*
 * ----------------------------------------------------------------------------
 * FUNCTIONS
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Compute and integration step of a system
 * @param system the SystemDescriptor of the system
 * @param x0 initial conditions
 * @param params system parameters
 * @param step integration step size
 * @param method integration algorithm; avaliable:
 *               - "euler": Euler's method
 *               - "rk3": Runge-Kutta 3rd order
 * @return mat  the system value at next step
 */
IntegrationResult integrate(SystemDescriptor &system, mat x0, mat params,
                            double step, std::string method = "euler");

/**
 * @brief Computes the traiectory of the system for the next n_step steps.
 *
 * @param system: the SystemDescriptor of the system
 * @param t0: the initial time
 * @param x0: the COLUMN array initial conditions
 * @param n_points: number of integration steps
 * @param step: step size
 * @param method: the algorithm to be used; possible values:
 *                - "euler": Euler's method
 *                - "rk3": Runge-Kutta 3rd order
 * @returns an IntegrationResult struct with next value and event if any
 */

mat traiectory(SystemDescriptor &system, mat x0, mat params,
               uint n_points, double step, std::string method = "euler");

/**
 * @brief The jacboian of the f function
 *
 * @param f function whose jacobian must be computed
 * @param x coordinate value
 * @param params parameters value
 * @param h derivative step
 * @return mat jacobian matrix
 */
mat jacobian(System_func f, mat x, mat params, double h = 1e-6);

// mat saltation_matrix(SystemDescriptor &system, mat x_previous,
//                      mat x_next, mat params, uint label1, uint label2);

// /**
//  * @brief The transition matrix calculate over a period T
//  *        Teoretically the monodromy matrix is calculate over a period but this
//  *        function calculate this kind of matrix over any T, be it the exact
//  *        period or not so the name may be not exact.
//  *
//  * @param system the system descriptor
//  * @param X0 initial conditions
//  * @param params system params
//  * @param T period
//  * @param step integration step size
//  * @param method integration algorith; available:
//  *               - "euler": first order implicit Euler method,
//  *               - "rk3": Runge-Kutta 3rd order method
//  * @return mat the transition matrix
//  */
// mat transition_matrix(SystemDescriptor &system, mat X0, mat params, double T,
//                       double step, bool save_traiectory = false, mat *traiectory = nullptr, std::string method = "euler");

// /**
//  * @brief Shooting algorithm for finding the period T of a limit cycle
//  *
//  * @param system system descriptor object
//  * @param x0 initial condition
//  * @param params system params
//  * @param T the starting guess for period
//  * @param step integration step
//  * @param max_iters max number of shooting trial
//  * @param save_traiectory if true must provide a pointer to a mat wher to store
//  * @param traiectory the pointer to a already instantiated matrix
//  * @param method integration algorithm, available:
//  *               - "euler": Euler's method,
//  *               - "rk3": Runge-Kutta 3rd order
//  * @return mat the monodromy matrix
//  */
// mat shooting(SystemDescriptor &system, mat x0, mat params, double T,
//              double step, uint max_iters, bool save_traiectory = false,
//              mat *traiectory = nullptr, std::string method = "euler");

// TODO change implementation for avoiding copy of return vector
mat **vector_field_2d(SystemDescriptor &system, double xmin,
                      double xmax, double ymin, double ymax,
                      uint x_points, uint y_points, mat params);

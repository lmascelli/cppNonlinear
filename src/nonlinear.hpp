#pragma once

#include <armadillo>
#include <string>
using arma::mat;

using uint = unsigned int;

/**
 * system_func
 * mat system_func(double t, mat x, mat params)
 *
 * Type representing a function that describe a system
 * @param t explicit time
 * @param x COLUMN array with point where to evaluate the system
 * @param params COLUMN array containg the paramaters value of the system
 * @return the value of the system at time T, coords X, parameters PARAMS
 */
using system_func = mat (*)(double, mat, mat);
/**
 * event_func
 * uint system_func(double t, mat X)
 *
 * This is a function evaluating the region of space where different
 * functions rule the system.
 * @param t time
 * @param x coords
 * @return a label rapresenting the region where the system is.
 */
using event_func = uint (*)(double, mat);

/**
 * event_struct
 * This struct hold the result of an event found by the integrate function
 * and contains the EVENT label, the time T at which the event occurred and
 * the X value it was.
 */
struct event_struct {
  uint event; // label rapresenting the manifold intersected
  double t;   // time of event occurred
  mat x;      // value of the traiectory at event releaved
};

/**
 * mat integrate(system_func f, double t0, mat x0, mat args, double h,
 * uint n_steps = 10, event_func event = nullptr,
 * event_struct *result =nullptr, string method = "newton")
 *
 * This function compute a step in interating the system looking if there
 * is an intersaction with a manifold if EVENT_FUNC is passed, returning
 * the next value and setting event related info in RESULT.
 *
 * @param f the function ruling the system
 * @param t0 initial time
 * @param x0 initial condition
 * @param args COLUMN vector with other argument of f (parameters)
 * @param h the step size
 * @param n_step the number of sub interval the H step will equally divided
 * @param event the function for checking manifold intersection
 * @param result a pointer to a EVENT_STRUCT where to store the event result
 * @param method the algorithm to be used; avaliable:
 *        - "newton": Newton's method
 *        - "rk3": Runge-Kutta 3rd order
 * @return the value of the system at next step
 */
mat integrate(system_func f, double t0, mat x0, mat args, double h,
              uint n_steps = 10, event_func event = nullptr,
              event_struct *result = nullptr, std::string method = "newton");

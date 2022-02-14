#pragma once

#include <armadillo>
#include <string>
#include <vector>
using arma::mat;

/*
 * ----------------------------------------------------------------------------
 * TYPES DECLARATION
 * ----------------------------------------------------------------------------
 */

using uint = uint64_t;

/**
 * @brief Type representing a function that describe a system
 * @param t explicit time
 * @param x COLUMN array with point where to evaluate the system
 * @param params COLUMN array containg the paramaters value of the system
 * @return the value of the system at time T, coords X, parameters PARAMS
 */
using system_func = mat (*)(double, mat, mat);
/**
 * @brief This is a function evaluating the region of space where different
 *        functions rule the system.
 * @param t time
 * @param x coords
 * @return a label rapresenting the region where the system is.
 */
using event_func = uint (*)(double, mat);

/*
 * ----------------------------------------------------------------------------
 * STRUCTURES AND CLASSED
 * ----------------------------------------------------------------------------
 */

/**
 * This struct hold the result of an event found by the integrate function
 * and contains the EVENT label, the time T at which the event occurred and
 * the X value it was.
 */
struct EventStruct
{
  uint event; // label rapresenting the manifold intersected
  double t;   // time of event occurred
  mat x;      // value of the traiectory at event releaved
};

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
   * @brief Insert a function in the system
   * @param f the function to insert
   * @param t the type of the function, default EQUATION
   */
  void AddFunction(system_func f, FunctionType t = FunctionType::EQUATION);
  /**
   * @brief Returns the ith function of the system
   * @param i the index of the function
   * @return The system function associated to that index
   */
  system_func GetFunction(const uint i) const;

  /**
   * @brief Ge
   *
   * @param i the index of the function
   * @return FunctionType The FunctionType of the ith function;
   */
  FunctionType GetType(const uint i) const;

  event_func manifold; // the function that given a point in the state-time
                       // parameter returns the index of the corresponding
                       // equation int vector F.

private:
  std::vector<system_func> F; // the equation ruling the system
  std::vector<FunctionType> Ft;
};

/*
 * ----------------------------------------------------------------------------
 * FUNCTIONS
 * ----------------------------------------------------------------------------
 */

/**
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
              EventStruct *result = nullptr, std::string method = "newton");

/**
 * @bried Computes the traiectory of the system from time t0 to time
 *        t0 + n_step * step
 * @param system: the SystemDescriptor of the system
 * @param t0: the initial time
 * @param x0: the COLUMN array initial conditions
 * @param n_points: number of integration steps
 * @param step: step size
 * @param method: the algorithm to be used; possible values:
 *                - "euler": Euler's method
 *                - "rk3": Runge-Kutta 3rd order
 * @returns a matrix with n_steps column made such:
 *          [t
 *           x]
 */
mat traiectory(SystemDescriptor system, double t0, mat x0, mat params, uint n_points,
               double step, std::string method = "newton");

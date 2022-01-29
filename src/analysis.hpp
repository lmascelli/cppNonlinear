#pragma once
#include <armadillo>
using namespace arma;

#include <string>
using std::string;
using uint = unsigned int;

using systemFunction = mat (*)(double, mat, mat);

class Analysis
{
public:
  /**
   * @brief Construct a new Analysis object
   *        
   * 
   * @param init_func: pointer to a function that initialize
   *                   the properties of the analysis.
   *                   The function itself take as parameter
   *                   a reference to the instance to initialize
   */
  Analysis(void (*init_func)(Analysis &self));

  Analysis() = delete;

  /**
   * @brief This function gets rid of the transient of the system
   * 
   * @param param column vector with the param value of the sistem
   * @param saveData true if you want to save the transient data
   * @param data a pointer to a matrix where to save the transient data
   * @return mat with the value at the end of the transient
   */
  mat transient(mat param, bool saveData = false, mat **data = nullptr);

  double t0;          // initial time
  double t_transient; // final time of the transient
  double tend;        // final time
  mat x0;             // initial conditions

  uint ntransient; // number of transient iterations
  uint ndata;      // number of pixel in equilibrium space

  systemFunction system; // pointer to the system functions
  systemFunction map;    // pointer to the map function

  uint (*manifold)(double, mat); // the switching manifold function

private:
  uint current_state; // current function ruling the system
  mat x_data;         // result of analysis
};

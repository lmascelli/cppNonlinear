#pragma once
#include <armadillo>
using namespace arma;

#include <string>
using std::string;
using uint = unsigned int;

class Analysis {
public:
  /*
  Analysis(void (*init_func) (Analysis &self))
  CONSTRUCTOR
  params:
       INIT_FUNC: function pointer to an initializing function
                  that must init the parameters of an analysis
                  class instance whose reference is given as
                  function parameter
  */
  Analysis(void (*init_func)(Analysis &self));
  Analysis() = delete;
  mat transient(mat param, bool saveData = false, mat *data = nullptr);

  string type; // "discrete" or "continue" system type

  mat params;   // range of param space
  uint nparams; // number of pixels in param space
  double t0;    // initial time or iteration number (n)
  double tend;
  mat x0; // initial conditions

  uint ntransient; // number of transient iterations
  uint ndata;      // number of x value for each param

  mat (*system)(double t, mat x, mat args); // system function pointer

private:
  mat x_data; // result of analysis
};

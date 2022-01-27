#include "src/analysis.hpp"
#include "src/nonlinear.hpp"

mat logistic_map(double n, mat xn, void *args) {
  double mu = *(double *)args;
  return mu * xn * (1 - xn);
}

int main(int argc, char const *argv[]) {
  Analysis a([](Analysis &self) {
    self.ndata = 1e3;
    self.ntransient = 2e3;
    self.nparam = 1e3;
    self.param_ranges = {2.4, 4};
    self.x0 = 0.5;
    self.system = logistic_map;
    self.type = "discrete";
  });
  a.run();
  return 0;
}

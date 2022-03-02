#include "utils.hpp"
#include "global.hpp"
#include <stdlib.h>

std::ostream &Log::Print() { return std::cout; }

void Log::print_shape(arma::mat &m, std::string name){
  Print() << name << "(" << m.n_rows << ", " << m.n_cols << ")\n";
}

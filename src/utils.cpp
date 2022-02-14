#include "utils.hpp"
#include <stdlib.h>

std::ostream &Log::Print() { return std::cout; }

void Plotting::PlotMat(arma::mat &m)
{
    system("python visualize.py");
}
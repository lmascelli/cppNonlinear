#include "utils.hpp"
#include "global.hpp"
#include <stdlib.h>

std::ostream &Log::Print() { return std::cout; }

void Plotting::PlotMat(arma::mat &m) {
  if (m.save("temp.csv", arma::csv_ascii)) {
#ifdef LINUX
    system("python3 visualize.py");
#elif defined(WINDOWS)
    system("python visualize.py");
#endif
  } else
    Log::Print() << "ERROR with plotting of matrix." << endl;
}

void Plotting::PlotCSV(std::string &filename) {
#ifdef LINUX
  system(("python3 visualize.py " + filename).c_str());
#elif defined(WINDOWS)
  system(("python visualize.py " + filename).c_str());
#endif
}

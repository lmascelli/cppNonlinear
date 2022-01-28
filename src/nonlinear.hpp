#pragma once

#include <armadillo>
using arma::mat;

#include <functional>
#include <iostream>
using std::cout, std::endl, std::string, std::function;

using uint = unsigned int;

mat integrate(function<mat(double, mat, mat)> f, double t0, mat x0, mat args,
              double h, uint n_steps = 10, function<mat(double, mat)> event, string method = "newton");

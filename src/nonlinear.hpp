#pragma once

#include <armadillo>
using arma::mat;

#include <functional>
#include <iostream>
using std::cout, std::endl, std::string, std::function;

using uint = unsigned int;

mat integrate(function<mat(double, mat, void *)> f, double t0, mat x0,
              void *args, double h, uint n_steps, string method = "newton");


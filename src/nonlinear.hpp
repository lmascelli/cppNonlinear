#pragma once

#include <armadillo>
using arma::mat;

#include <functional>
#include <iostream>

using std::cout, std::endl, std::string, std::function;

using uint = unsigned int;

struct event_struct
{
    uint event;
    double t;
    mat x;
};

mat integrate(function<mat(double, mat, mat)> f,
              double t0, mat x0, mat args,
              double h,
              uint n_steps = 10,
              function<uint(double, mat)> event = nullptr,
              event_struct *result = nullptr,
              string method = "newton");

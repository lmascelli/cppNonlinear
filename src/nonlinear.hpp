#pragma once

#include <armadillo>
using arma::mat;

#include <iostream>

using std::cout, std::endl, std::string;

using uint = unsigned int;

struct event_struct
{
    uint event;
    double t;
    mat *x;
    Event_struct();
    ~Event_struct();
};

mat integrate(mat (*f)(double, mat, mat),
              double t0, mat x0, mat args,
              double h,
              uint n_steps = 10,
              uint (*event)(double, mat) = nullptr,
              event_struct *result = nullptr,
              string method = "newton");

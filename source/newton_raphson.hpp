#pragma once

#include <functional>
#include <iostream>
#include <limits>

#include <cmath>

#include "log.hpp"

/* ******************************************************************** Synopsis

The Newton-Raphson algorithm is used by the application to solve equations for
semiconductors, where finding the x-axis intercepts of an IV characteristic is
required.

Read more on the topic here: https://en.wikipedia.org/wiki/Newton%27s_method

*/

namespace NewtonRaphson {

// This limits the number of attempts which the function will make at refining
// the result before giving up. The higher the iteration bound, the higher the
// precision, but the slower the function
static const unsigned int iteration_bound = 50;

// These values are used every time the function is called, so it makes sense to
// have them as scope-limited global constants
static const auto epsilon = std::numeric_limits<double>::epsilon();
static const auto double_maximum = std::numeric_limits<double>::max();

// Calculate the closes x-axis intercept to a function, given its derivative
// NOTE: This function will fail if the gradient at the start point provided is
// zero
// TODO: Maybe alter the function to nudge the start point in this particular
// scenario? Shouldn't be an issue for diode characteristic curves, but...
double evaluate(const std::function<double(double)> &function,
        const std::function<double(double)> &derivative,
        const double &start_point,
        const double precision = epsilon) {

    double previous_result = start_point;
    double current_result = 0;
    double delta = double_maximum;

    unsigned int index = 0;

    while(std::fabs(delta - 0) >= precision && index < iteration_bound) {
        if(std::fabs(derivative(previous_result) - 0) < epsilon) {
            Log::error() << "Can't start Newton-Raphson iteration at a point "
                    "where the derivative is zero" << std::endl;
            throw -1;
        }

        current_result = previous_result - function(previous_result) /
                derivative(previous_result);
        delta = current_result - previous_result;
        previous_result = current_result;

        index += 1;
    }

    return current_result;
}

}; // NewtonRaphson

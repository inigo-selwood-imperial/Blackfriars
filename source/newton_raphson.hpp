#pragma once

#include <functional>
#include <limits>

namespace NewtonRaphson {

// The maximum number of iterations allowed before returning a value
static const unsigned int iteration_bound = 50;

// Constants declared here so that they needn't be called every time the
// function is evaluated
static const auto epsilon = std::numeric_limits<double>::epsilon();
static const auto double_maximum = std::numeric_limits<double>::max();

// Calculate the x-axis intercept of a function, given its derivative, and
// a starting point.
// NOTE: This function will throw an exception if the gradient at the given
// start position is zero
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
            std::cerr << "Can't start Newton-Raphson iteration at a point "
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

}; // NewtonRaphson\

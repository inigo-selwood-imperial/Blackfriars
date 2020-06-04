#pragma once

#include <functional>
#include <iostream>
#include <limits>

#include <cmath>

namespace NewtonRaphson {

static const unsigned int iteration_bound = 50;

static const auto epsilon = std::numeric_limits<double>::epsilon();
static const auto double_maximum = std::numeric_limits<double>::max();

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

}; // NewtonRaphson

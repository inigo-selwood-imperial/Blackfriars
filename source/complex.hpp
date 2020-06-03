#pragma once

#include <ostream>

#include <cmath>

class Complex {

public:

    double real_part;

    double imaginary_part;

    Complex &operator-=(const Complex &value);
    Complex &operator-=(const double &value);
    Complex &operator*=(const Complex &value);
    Complex &operator*=(const double &factor);
    Complex &operator/=(const Complex &value);
    Complex &operator/=(const double &factor);
    Complex &operator+=(const Complex &value);
    Complex &operator+=(const double &value);

    Complex &operator=(const double &value);

    Complex();
    Complex(const double &real_part);
    Complex(const double &real_part, const double &imaginary_part);

    Complex power(const int &power);

    double magnitude() const;
    double theta() const;

};

std::ostream &operator<<(std::ostream &stream, const Complex &complex);

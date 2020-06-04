#pragma once

#include <limits>
#include <ostream>

#include <cmath>

/* ******************************************************************** Synopsis

The complex number class is just what it sounds like; a representation of a
complex number in Cartesian form. It provides operator overrides for complex
number maths (operations like addition, multiplication, etc).

The source file is split up into 3 parts:

    (1) Definition
        Outline of the Complex class and its methods

    (2) Operators
        Arithmetic and equivalence operator overrides

    (3) Constructors

*/

// ****************************************************************** Definition

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

};

std::ostream &operator<<(std::ostream &stream, const Complex &complex);

bool operator!=(const Complex &one, const Complex &two);
bool operator==(const Complex &one, const Complex &two);

Complex operator-(const Complex &one, const Complex &two);
Complex operator-(const Complex &one, const double &value);
Complex operator*(const Complex &one, const Complex &two);
Complex operator*(const Complex &one, const double &factor);
Complex operator/(const Complex &one, const Complex &two);
Complex operator/(const Complex &one, const double &factor);
Complex operator+(const Complex &one, const Complex &two);
Complex operator+(const Complex &one, const double &value);

// ******************************************************************* Operators

// Serializes a complex number to a stream, in Cartesian form
std::ostream &operator<<(std::ostream &stream, const Complex &complex) {
    bool real_part_printed = false;
    const auto epsilon = std::numeric_limits<double>::epsilon();
    if(std::fabs(std::fabs(complex.real_part) - 0) >= epsilon) {
        stream << complex.real_part;
        real_part_printed = true;
    }

    if(std::fabs(complex.imaginary_part - 0.0) < epsilon)
        return stream;
    else if(real_part_printed)
        stream << ((complex.imaginary_part < 0) ? " - " : " + ");

    if(real_part_printed == false)
        stream << complex.imaginary_part;
    else if(std::fabs(std::fabs(complex.imaginary_part) - 1.0) < epsilon)
        stream << 'i';
    else
        stream << std::fabs(complex.imaginary_part) << "i";

    return stream;
}

// True if two complex numbers aren't equal
bool operator!=(const Complex &one, const Complex &two) {
    return (one == two) == false;
}

// True if two complex numbers are equal
bool operator==(const Complex &one, const Complex &two) {
    const auto epsilon = std::numeric_limits<double>::epsilon();
    return std::fabs(one.real_part - two.real_part) < epsilon &&
            std::fabs(one.imaginary_part - two.imaginary_part) < epsilon;
}

// Subtracts two complex numbers
Complex operator-(const Complex &one, const Complex &two) {
    return Complex(one) -= two;
}

// Subtracts a constant value from the real part of a complex number
Complex operator-(const Complex &one, const double &value) {
    return Complex(one) -= value;
}

// Multiplies two complex numbers together
Complex operator*(const Complex &one, const Complex &two) {
    return Complex(one) *= two;
}

// Multiplies a complex number's real and imaginary parts by a factor
Complex operator*(const Complex &one, const double &factor) {
    return Complex(one) *= factor;
}

// Divides two complex numbers by one another
Complex operator/(const Complex &one, const Complex &two) {
    return Complex(one) /= two;
}

// Divides the real and imaginary parts of a complex number by a factor
Complex operator/(const Complex &one, const double &factor) {
    return Complex(one) /= factor;
}

// Adds two complex numbers together
Complex operator+(const Complex &one, const Complex &two) {
    return Complex(one) += two;
}

// Adds a value to the real part of a complex number
Complex operator+(const Complex &one, const double &value) {
    return Complex(one) += value;
}

// Subtracts a complex value from this instance
Complex &Complex::operator-=(const Complex &value) {
    real_part -= value.real_part;
    imaginary_part -= value.imaginary_part;
    return *this;
}

// Subtracts a value from this instance's real part
Complex &Complex::operator-=(const double &value) {
    real_part -= value;
    return *this;
}

// Multiplies this instance by another complex number
Complex &Complex::operator*=(const Complex &value) {
    Complex result;
    result.real_part = (real_part * value.real_part) -
            (imaginary_part * value.imaginary_part);
    result.imaginary_part = (imaginary_part * value.real_part) +
            (real_part * value.imaginary_part);
    *this = result;
    return *this;
}

// Multiplies this instance's real and imaginary parts by a factor
Complex &Complex::operator*=(const double &factor) {
    real_part *= factor;
    imaginary_part *= factor;
    return *this;
}

// Divides this instance by another complex number
Complex &Complex::operator/=(const Complex &value) {
    const double denominator = std::pow(value.real_part, 2) +
            std::pow(value.imaginary_part, 2);

    Complex result;

    result.real_part = ((real_part * value.real_part) +
            (imaginary_part * value.imaginary_part)) / denominator;
    result.imaginary_part = ((imaginary_part * value.real_part) -
            (real_part * value.imaginary_part)) / denominator;

    *this = result;
    return *this;
}

// Divides this instance's real and complex parts be a factor
Complex &Complex::operator/=(const double &factor) {
    real_part /= factor;
    imaginary_part /= factor;
    return *this;
}

// Adds another complex value to this instance
Complex &Complex::operator+=(const Complex &value) {
    real_part += value.real_part;
    imaginary_part += value.imaginary_part;
    return *this;
}

// Adds a scalar value to this instance's real part
Complex &Complex::operator+=(const double &value) {
    real_part += value;
    return *this;
}

// Assignment operator
Complex &Complex::operator=(const double &value) {
    real_part = value;
    return *this;
}

// **************************************************************** Constructors

Complex::Complex() {
    real_part = 0;
    imaginary_part = 0;
}

Complex::Complex(const double &real_part) {
    this->real_part = real_part;
    this->imaginary_part = 0;
}

Complex::Complex(const double &real_part, const double &imaginary_part) {
    this->real_part = real_part;
    this->imaginary_part = imaginary_part;
}

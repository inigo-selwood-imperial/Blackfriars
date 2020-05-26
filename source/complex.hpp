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

    Complex();
    Complex(const double &real_part, const double &imaginary_part);

    double magnitude() const;
    double theta() const;

};

// ******************************************************* Stream print operator

std::ostream &operator<<(std::ostream &stream, const Complex &complex) {
    return stream << complex.real_part <<
            ((complex.imaginary_part < 0) ? " - " : " + ") <<
            std::abs(complex.imaginary_part) << 'i';
}

// ************************************************************ Binary operators

Complex operator-(const Complex &one, const Complex &two) {
    return Complex(one) -= two;
}

Complex operator-(const Complex &one, const double &value) {
    return Complex(one) -= value;
}

Complex operator*(const Complex &one, const Complex &two) {
    return Complex(one) *= two;
}

Complex operator*(const Complex &one, const double &factor) {
    return Complex(one) *= factor;
}

Complex operator/(const Complex &one, const Complex &two) {
    return Complex(one) /= two;
}

Complex operator/(const Complex &one, const double &factor) {
    return Complex(one) /= factor;
}

Complex operator+(const Complex &one, const Complex &two) {
    return Complex(one) += two;
}

Complex operator+(const Complex &one, const double &value) {
    return Complex(one) += value;
}

// ****************************************************** Unary member operators

Complex &Complex::operator-=(const Complex &value) {
    real_part -= value.real_part;
    imaginary_part -= value.imaginary_part;
    return *this;
}

Complex &Complex::operator-=(const double &value) {
    real_part -= value;
    return *this;
}

Complex &Complex::operator*=(const Complex &value) {
    Complex result;
    result.real_part = (real_part * value.real_part) -
            (imaginary_part * value.imaginary_part);
    result.imaginary_part = (imaginary_part * value.real_part) +
            (real_part * value.imaginary_part);
    *this = result;
    return *this;
}

Complex &Complex::operator*=(const double &factor) {
    real_part *= factor;
    imaginary_part *= factor;
    return *this;
}

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

Complex &Complex::operator/=(const double &factor) {
    real_part /= factor;
    imaginary_part /= factor;
    return *this;
}

Complex &Complex::operator+=(const Complex &value) {
    real_part += value.real_part;
    imaginary_part += value.imaginary_part;
    return *this;
}

Complex &Complex::operator+=(const double &value) {
    real_part += value;
    return *this;
}

Complex::Complex() {
    real_part = 0;
    imaginary_part = 0;
}

Complex::Complex(const double &real_part, const double &imaginary_part) {
    this->real_part = real_part;
    this->imaginary_part = imaginary_part;
}

double Complex::magnitude() const {
    return std::sqrt(std::pow(real_part, 2) + std::pow(imaginary_part, 2));
}

double Complex::theta() const {
    return std::atan(imaginary_part / real_part);
}
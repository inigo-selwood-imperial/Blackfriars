#pragma once

class Complex {

private:

    double real_part;

    double complex_part;

public:

    Complex &operator-=(const Complex &value);
    Complex &operator-=(const double &value);
    Complex &operator*=(const Complex &value);
    Complex &operator*=(const double &factor);
    Complex &operator/=(const Complex &value);
    Complex &operator/=(const double &factor);
    Complex &operator+=(const Complex &value);
    Complex &operator+=(const double &value);

    Complex();

    double magnitude() const;
    double theta() const;

};

// ******************************************************* Stream print operator

std::ostream &operator<<(std::ostream &stream, const Complex &complex) {
    return stream << real_part << ((complex_part < 0) ? " - " : " + ") <<
            std::abs(complex_part) << 'i';
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
    real_part -= complex.real_part;
    complex_part -= complex.complex_part;
    return *this;
}

Complex &Complex::operator-=(const double &value) {
    real_part -= value;
    return *this;
}

Complex &Complex::operator*=(const Complex &value) {
    real_part = (real_part * value.real_part) -
            (complex_part * value.complex_part);
    complex_part = (real_part * complex_part) +
            (value.real_part * value.complex_part);
    return *this;
}

Complex &Complex::operator*=(const double &factor) {
    real_part *= factor;
    complex_part *= factor;
    return *this;
}

Complex &Complex::operator/=(const Complex &value) {
    const double denominator = std::pow(value.real_part, 2) +
            std::pow(value.complex_part, 2);

    real_part = ((real_part * value.real_part) +
            (complex_part * value.complex_part)) / denominator;
    complex_part = ((complex_part * value.real_part) -
            (real_part * value.complex_part)) / denominator;

    return *this;
}

Complex &Complex::operator/=(const double &factor) {
    real_part /= factor;
    complex_part /= factor;
    return *this;
}

Complex &Complex::operator+=(const Complex &value) {
    real_part += value.real_part;
    complex_part += value.complex_part;
    return *this;
}

Complex &Complex::operator+=(const double &value) {
    real_part += value;
    return *this;
}

Complex::Complex() {
    real_part = 0;
    complex_part = 0;
}

double Complex::magnitude() const {
    return std::sqrt(std::pow(real_part, 2) + std::pow(complex_part, 2));
}

double Complex::theta() const {
    return std::atan(complex_part / real_part);
}

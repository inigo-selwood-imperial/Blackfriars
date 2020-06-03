#pragma once

#include <array>
#include <iostream>
#include <limits>
#include <ostream>
#include <vector>

#include <cmath>

#include "complex.hpp"

/*
The matrix class is perhaps the most important of all in this application. It
is used to solve the simulateous Kirchhoff equations for all the components in
the circuit.

Since some of the components in a circuit have complex values (capacitors and
inductors), the matrix class stores its values as instances of the Complex class
(see complex.hpp for more details).

It provides overload operators for all the standard mathematical operations, as
well as dedicated functions for finding the following properties of a matrix:
    - adjugate
    - cofactor
    - determinant
    - inverse
    - transpose

The values of the matrix are stored in row-major order, meaning that the
values are accessed as such:

    matrix = | 00 01 ... 0n |
             | 10 11 ... 1n |
             |       ...    |
             | m0 m1 ... mn |

TODO: Guard against operations on matrices with width or height 1
*/

class Matrix {

public:

    typedef std::array<unsigned int, 2> Size;
    typedef std::array<unsigned int, 2> Index;

    static Matrix identity(const unsigned int &size);

    Complex &operator()(const unsigned int &column, const unsigned int &row);
    Complex operator()(const unsigned int &column, const unsigned int &row)
            const;
    Matrix operator()(const unsigned int &column_one,
            const unsigned int &row_one, const unsigned int &column_two,
            const unsigned int &row_two) const;

    Matrix &operator*=(const double &factor);
    Matrix &operator*=(const Matrix &matrix);
    Matrix &operator/=(const double &factor);
    Matrix &operator+=(const Matrix &matrix);

    Matrix();
    Matrix(const Complex &value);
    Matrix(const std::vector<std::vector<Complex>> &values);
    Matrix(const std::initializer_list<std::vector<Complex>> &values);
    Matrix(const unsigned int &rows, const unsigned int &columns);

    void resize(const unsigned int &rows, const unsigned int &columns);
    void clear();

    Matrix &remove_column(const unsigned int &column);
    Matrix &remove_row(const unsigned int &row);

    Complex determinant() const;
    Matrix adjugate() const;
    Matrix cofactor() const;
    Matrix inverse() const;
    Matrix transpose() const;

    unsigned int columns() const;
    unsigned int rows() const;
    Size size() const;

    unsigned int volume() const;

    std::vector<Complex> values() const;
    std::vector<Complex> &values();

private:

    std::vector<Complex> _values;

    unsigned int _columns;
    unsigned int _rows;

    Index index(const unsigned int &offset) const;
    unsigned int offset(const Index &index) const;
    unsigned int offset(const unsigned int &row, const unsigned int &column)
            const;

};

std::ostream &operator<<(std::ostream &stream, const Matrix &matrix);
std::ostream &operator<<(std::ostream &stream,
        const std::array<unsigned int, 2> &index);

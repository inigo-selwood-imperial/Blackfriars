#pragma once

#include <array>
#include <limits>
#include <ostream>
#include <vector>

#include <cmath>

class Matrix {

public:

    typedef std::array<unsigned int, 2> Size;
    typedef std::array<unsigned int, 2> Index;

    static Matrix identity(const unsigned int &size);

    static Matrix power(const Matrix &matrix, const unsigned int &exponent);

    double &operator()(const unsigned int &column, const unsigned int &row);
    double operator()(const unsigned int &column, const unsigned int &row)
            const;

    Matrix &operator*=(const double &factor);
    Matrix &operator*=(const Matrix &matrix);
    Matrix &operator/=(const double &factor);
    Matrix &operator/=(const Matrix &matrix);
    Matrix &operator+=(const Matrix &matrix);

    Matrix();
    Matrix(const std::vector<std::vector<double>> &values);
    Matrix(const unsigned int &rows, const unsigned int &columns);

    void resize(const unsigned int &rows, const unsigned int &columns);
    void clear();

    double determinant() const;
    Matrix inverse() const;
    Matrix transpose() const;

    unsigned int columns() const;
    unsigned int rows() const;
    Size size() const;

    unsigned int volume() const;

    std::vector<double> values() const;
    std::vector<double> &values();

private:

    std::vector<double> _values;

    unsigned int _columns;
    unsigned int _rows;

    Index index(const unsigned int &offset) const;
    unsigned int offset(const Index &index) const;
    unsigned int offset(const unsigned int &row, const unsigned int &column)
            const;

};

// Prints an matrix to a stream
std::ostream &operator<<(std::ostream &stream, const Matrix &matrix) {
    const unsigned int columns = matrix.columns();
    for(unsigned int row = 0; row < matrix.rows(); row += 1) {
        stream << "[";

        for(unsigned int column = 0; column < columns; column += 1) {
            stream << matrix(row, column);
            if((column + 1) < columns)
                stream << ", ";
        }
        stream << "]" << std::endl;
    }

    return stream;
}

// Prints an index to a stream
std::ostream &operator<<(std::ostream &stream,
        const std::array<unsigned int, 2> &index) {

    return stream << "(" << index[0] << ", " << index[1] << ")";
}

// Returns a matrix multiplied by a scalar factor
Matrix operator*(const Matrix &matrix, const double &factor) {
    return Matrix(matrix) *= factor;
}

// Returns the product of two matrices
Matrix operator*(const Matrix &one, const Matrix &two) {
    return Matrix(one) *= two;
}

// Returns a matrix divided by a scalar factor
Matrix operator/(const Matrix &matrix, const double &factor) {
    return Matrix(matrix) /= factor;
}

// Returns the sum of two matrices
Matrix operator+(const Matrix &one, const Matrix &two) {
    return Matrix(one) *= two;
}

// Returns true if the matrices are equal
bool operator==(const Matrix &one, const Matrix &two) {
    if(one.size() != two.size())
        return false;

    const auto one_values = one.values();
    const auto two_values = two.values();
    auto epsilon = std::numeric_limits<double>::epsilon();
    for(unsigned int index = 0; index < one.volume(); index += 1) {
        if(std::fabs(one_values[index] - two_values[index]) >= epsilon)
            return false;
    }

    return true;
}

// Returns true if the two matrices aren't equal
bool operator!=(const Matrix &one, const Matrix &two) {
    return (one == two) == false;
}

// Returns the identity matrix of a given size
Matrix Matrix::identity(const unsigned int &size) {
    Matrix matrix(size, size);
    for(unsigned int index = 0; index < size; index += 1)
        matrix(index, index) = 1;
    return matrix;
}

// Returns the result of a matrix to a given power
Matrix Matrix::power(const Matrix &matrix, const unsigned int &exponent) {
    return Matrix();
}

// Returns the value at a given column and row index
double &Matrix::operator()(const unsigned int &row,
        const unsigned int &column) {

    return _values[offset(row, column)];
}

// Returns the value at a given column and row index
double Matrix::operator()(const unsigned int &row, const unsigned int &column)
        const {

    return (*this)(row, column);
}

// Multiplies the matrix by a scalar factor
Matrix &Matrix::operator*=(const double &factor) {
    for(auto &value : _values)
        value *= factor;
    return *this;
}

// Multiplies the matrix by another matrix
Matrix &Matrix::operator*=(const Matrix &matrix) {
    if(_columns != matrix.rows())
        throw -1;

    Matrix result(_rows, matrix.columns());
    for(unsigned int row = 0; row < _rows; row += 1) {
        for(unsigned int column = 0; column < matrix.columns(); column += 1) {
            double sum = 0;
            for(unsigned int index = 0; index < _columns; index += 1)
                sum += (*this)(row, index) * matrix(index, column);
            result(row, column) = sum;
        }
    }

    *this = result;
    return *this;
}

// Divides the matrix by a scalar factor
Matrix &Matrix::operator/=(const double &factor) {
    for(auto &value : _values)
        value /= factor;
    return *this;
}

// Divides the matrix by another matrix
Matrix &Matrix::operator/=(const Matrix &matrix) {
    throw -1;
}

// Adds a matrix to this matrix
Matrix &Matrix::operator+=(const Matrix &matrix) {
    if(this->size() != matrix.size())
        throw -1;

    const auto values = matrix.values();
    for(unsigned int index = 0; index < volume(); index += 1)
        _values[index] += values[index];
    return *this;
}

Matrix::Matrix() {
    _columns = 0;
    _rows = 0;
}

Matrix::Matrix(const std::vector<std::vector<double>> &values) {

}

Matrix::Matrix(const unsigned int &rows, const unsigned int &columns) {
    resize(rows, columns);
}

// Resizes the matrix, copying any overlapping data into the resized matrix
void Matrix::resize(const unsigned int &rows, const unsigned int &columns) {

    if(_values.empty()) {
        _rows = rows;
        _columns = columns;
        _values.resize(rows * columns, 0);

        return;
    }

    Matrix temporary(*this);

    _rows = rows;
    _columns = columns;

    _values.resize(rows * columns, 0);
    _values.clear();

    auto row_bound = std::min(_rows, temporary.rows());
    auto column_bound = std::min(_columns, temporary.columns());
    for(unsigned int row = 0; row < row_bound; row += 1) {
        for(unsigned int column = 0; column < column_bound; column += 1)
            (*this)(row, column) = temporary(row, column);
    }
}

// Clear the values in the matrix, leaving its size intact
void Matrix::clear() {
    for(auto &value : _values)
        value = 0;
}

// Returns the determinant of the matrix
double Matrix::determinant() const {
    return 0;
}

// Returns the inverse of the matrix
Matrix Matrix::inverse() const {
    return Matrix();
}

// Returns the transpose of the matrix
Matrix Matrix::transpose() const {
    return Matrix();
}

// Returns the number of columns in the matrix
unsigned int Matrix::columns() const {
    return _columns;
}

// Returns the number of rows in the matrix
unsigned int Matrix::rows() const {
    return _rows;
}

// Returns the size of the matrix, in the format {columns, rows}
Matrix::Size Matrix::size() const {
    return {_columns, _rows};
}

// Returns the volume of the matrix
unsigned int Matrix::volume() const {
    return _columns * _rows;
}

// Returns the index corresponding to a row-major order offset
Matrix::Index Matrix::index(const unsigned int &offset) const {
    return Index({offset / _rows, offset % _columns});
}

// Returns the row-major order offset of a given column, row index
unsigned int Matrix::offset(const unsigned int &row, const unsigned int &column)
        const {

    return offset({row, column});
}

// Returns the row-major order offset of a given index
unsigned int Matrix::offset(const Index &index) const {
    return offset(index[0], index[1]);
}

// Returns a reference the raw values contained by the matrix
std::vector<double> &Matrix::values() {
    return _values;
}

// Returns the raw values contained by the matrix
std::vector<double> Matrix::values() const {
    return _values;
}
#pragma once

#include <array>
#include <ostream>
#include <vector>

#include <cmath>

// TODO: Implement a custom iterator
class Matrix {

public:

    typedef std::array<unsigned int, 2> Size;
    typedef std::array<unsigned int, 2> Index;

    static Matrix identity_matrix(const unsigned int &size);

    static double dot_product(const Matrix &one, const Matrix &two);
    static Matrix cross_product(const Matrix &one, const Matrix &two);
    static Matrix power(const Matrix &matrix, const unsigned int &exponent);

    double &operator()(const unsigned int &column, const unsigned int &row);

    Matrix &operator*=(const double &factor);
    Matrix &operator/=(const double &factor);
    Matrix &operator+=(const Matrix &matrix);

    Matrix();
    Matrix(const std::vector<std::vector<double>> &values);
    Matrix(const unsigned int &columns, const unsigned int &rows);

    void resize(const unsigned int &columns, const unsigned int &rows);
    void clear();

    double determinant() const;
    Matrix inverse() const;
    Matrix transpose() const;

    Size size();
    unsigned int volume() const;

private:

    std::vector<double> values;

    unsigned int columns;
    unsigned int rows;

    Index index(const unsigned int &offset);
    unsigned int offset(const Index &index);
    unsigned int offset(const unsigned int &column, const unsigned int &row);
};

// Prints an matrix to a stream
std::ostream &operator<<(std::ostream &stream, const Matrix &matrix) {
    return stream;
}

// Prints an index to a stream
std::ostream &operator<<(std::ostream &stream,
        const std::array<unsigned int, 2> &index) {

    return stream;
}

// Returns a matrix multiplied by a scalar factor
Matrix operator*(const Matrix &matrix, const double &factor) {
    return Matrix();
}

// Returns a matrix divided by a scalar factor
Matrix operator/(const Matrix &matrix, const double &factor) {
    return Matrix();
}

// Returns the sum of two matrices
Matrix operator+(const Matrix &one, const Matrix &two) {
    return Matrix();
}

// Returns true if the two matrices aren't equal
bool operator!=(const Matrix &one, const Matrix &two) {
    return false;
}

// Returns true if the matrices are equal
bool operator==(const Matrix &one, const Matrix &two) {
    return false;
}

// Returns the identity matrix of a given size
Matrix Matrix::identity_matrix(const unsigned int &size) {
    return Matrix();
}

// Returns the dot product of two matrices
double Matrix::dot_product(const Matrix &one, const Matrix &two) {
    return 0;
}

// Returns the cross product of two matrices
Matrix Matrix::cross_product(const Matrix &one, const Matrix &two) {
    return Matrix();
}

// Returns the result of a matrix to a given power
Matrix Matrix::power(const Matrix &matrix, const unsigned int &exponent) {
    return Matrix();
}

// Returns the value at a given column and row index
double &Matrix::operator()(const unsigned int &column,
        const unsigned int &row) {

    throw -1;
}

// Multiplies the matrix by a scalar factor
Matrix &Matrix::operator*=(const double &factor) {
    throw -1;
}

// Divides the matrix by a scalar factor
Matrix &Matrix::operator/=(const double &factor) {
    throw -1;
}

// Adds a matrix to this matrix
Matrix &Matrix::operator+=(const Matrix &matrix) {
    throw -1;
}

Matrix::Matrix() {
    columns = 0;
    rows = 0;
}

Matrix::Matrix(const std::vector<std::vector<double>> &values) {}

Matrix::Matrix(const unsigned int &columns, const unsigned int &rows) {}

// Resizes the matrix, copying any overlapping data into the resized matrix
void Matrix::resize(const unsigned int &columns, const unsigned int &rows) {}

// Clear the values in the matrix, leaving its size intact
void Matrix::clear() {
    for(auto &value : values)
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

// Returns the size of the matrix, in the format {columns, rows}
Matrix::Size Matrix::size() {
    return {columns, rows};
}

// Returns the volume of the matrix
unsigned int Matrix::volume() const {
    return columns * rows;
}

// Returns the index corresponding to a row-major order offset
Matrix::Index Matrix::index(const unsigned int &offset) {
    unsigned int column = offset / columns;
    unsigned int row = offset % rows;
    return {column, row};
}

// Returns the row=major order offset of a given column, row index
unsigned int Matrix::offset(const unsigned int &column,
        const unsigned int &row) {

    return offset({column, row});
}

// Returns the row-major order offset of a given index
unsigned int Matrix::offset(const Index &index) {
    return index[0] * columns + index[1];
}

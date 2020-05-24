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

    static Matrix identity(const unsigned int &size);

    static double dot_product(const Matrix &one, const Matrix &two);
    static Matrix cross_product(const Matrix &one, const Matrix &two);
    static Matrix power(const Matrix &matrix, const unsigned int &exponent);

    double &operator()(const unsigned int &column, const unsigned int &row);
    double operator()(const unsigned int &column, const unsigned int &row)
            const;

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

    Size size() const;
    unsigned int volume() const;

private:

    std::vector<double> values;

    unsigned int columns;
    unsigned int rows;

    Index index(const unsigned int &offset) const;
    unsigned int offset(const Index &index) const;
    unsigned int offset(const unsigned int &column, const unsigned int &row)
            const;

};

// Prints an matrix to a stream
std::ostream &operator<<(std::ostream &stream, const Matrix &matrix) {
    const auto size = matrix.size();
    for(unsigned int row = 0; row < size[1]; row += 1) {
        stream << "[";
        for(unsigned int column = 0; column < size[0]; column += 1) {
            stream << matrix(column, row);
            if(column + 1 < size[0])
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
    Matrix result = matrix;
    return result *= factor;
}

// Returns a matrix divided by a scalar factor
Matrix operator/(const Matrix &matrix, const double &factor) {
    Matrix result = matrix;
    return result *= factor;
}

// Returns the sum of two matrices
Matrix operator+(const Matrix &one, const Matrix &two) {
    Matrix result = one;
    return result += two;
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
Matrix Matrix::identity(const unsigned int &size) {
    Matrix matrix(size, size);
    for(unsigned int index = 0; index < size; index += 1)
        matrix(index, index) = 1;
    return matrix;
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

    return values[offset(column, row)];
}

// Returns the value at a given column and row index
double Matrix::operator()(const unsigned int &column,
        const unsigned int &row) const {

    return values[offset(column, row)];
}

// Multiplies the matrix by a scalar factor
Matrix &Matrix::operator*=(const double &factor) {
    for(auto &value : values)
        value *= factor;
    return *this;
}

// Divides the matrix by a scalar factor
Matrix &Matrix::operator/=(const double &factor) {
    for(auto &value : values)
        value /= factor;
    return *this;
}

// Adds a matrix to this matrix
Matrix &Matrix::operator+=(const Matrix &matrix) {
    if(size() != matrix.size())
        throw -1;

    for(unsigned int column = 0; column < columns; column += 1) {
        for(unsigned int row = 0; row < rows; row += 1)
            (*this)(column, row) += matrix(column, row);
    }

    return *this;
}

Matrix::Matrix() {
    columns = 0;
    rows = 0;
}

Matrix::Matrix(const std::vector<std::vector<double>> &values) {
    unsigned int rows = 0;
    for(const auto &row : values)
        rows = std::max(row.size(), rows);
    resize(values.size(), rows);

    for(unsigned int column = 0; column < columns; column += 1) {
        for(unsigned int row = 0; row < rows; row += 1)
            (*this)(column, row) = values[column][row];
    }
}

Matrix::Matrix(const unsigned int &columns, const unsigned int &rows) {
    resize(columns, rows);
}

// Resizes the matrix, copying any overlapping data into the resized matrix
void Matrix::resize(const unsigned int &columns, const unsigned int &rows) {
    this->columns = columns;
    this->rows = rows;

    if(values.empty()) {
        values.resize(columns * rows);
        return;
    }

    Matrix temporary = *this;

    values.clear();
    values.resize(columns * rows);

    unsigned int column_bound = std::min(columns, temporary.columns);
    unsigned int row_bound = std::min(rows, temporary.rows);
    for(unsigned int column = 0; column < column_bound; column += 1) {
        for(unsigned int row = 0; row < row_bound; row += 1)
            (*this)(column, row) = temporary(column, row);
    }
}

// Clear the values in the matrix, leaving its size intact
void Matrix::clear() {
    for(auto &value : values)
        value = 0;
}

// Returns the determinant of the matrix
double Matrix::determinant() const {
    if(columns != rows)
        throw -1;

    const auto size = columns;

    Matrix lower_matrix(columns, rows);
    Matrix upper_matrix(columns, rows);

    std::array<unsigned int, 2> index;
    for(index[0] = 0; index[0] < size; index[0] += 1) {

        for(index[1] = 0; index[1] < size; index[1] += 1) {
            if(index[1] < index[0])
                lower_matrix(index[1], index[0]) = 0;

            else {
                lower_matrix(index[1], index[0]) = (*this)(index[1], index[0]);
                for(unsigned int offset = 0; offset < index[0]; offset += 1) {
                    lower_matrix(index[1], index[0]) =
                            lower_matrix(index[1], index[0]) -
                            lower_matrix(index[1], offset) *
                            upper_matrix(offset, index[0]);
                }
            }
        }

        for(index[1] = 0; index[1] < size; index[1] += 1) {
            if(index[1] < index[0])
                upper_matrix(index[0], index[1]) = 0;

            else if(index[1] == index[0])
                upper_matrix(index[0], index[1]) = 1;

            else {
                upper_matrix(index[0], index[1]) = (*this)(index[0], index[1]) /
                        lower_matrix(index[0], index[0]);
                for(unsigned int offset = 0; offset < index[0]; offset += 1) {
                    upper_matrix(index[0], index[1]) =
                            upper_matrix(index[0], index[1]) -
                            ((lower_matrix(index[0], offset) *
                            upper_matrix(offset, index[1])) /
                            lower_matrix(index[0], index[0]));
                }
            }
        }
    }

    double determinant = 1;
    for(unsigned int index = 0; index < size; index += 1) {
        determinant *= lower_matrix(index, index);
        determinant *= upper_matrix(index, index);
    }
    return determinant;
}

// Returns the inverse of the matrix
Matrix Matrix::inverse() const {
    return Matrix();
}

// Returns the transpose of the matrix
Matrix Matrix::transpose() const {
    Matrix result(rows, columns);
    for(unsigned int column = 0; column < columns; column += 1) {
        for(unsigned int row = 0; row < rows; row += 1)
            result(row, column) = (*this)(column, row);
    }
    return result;
}

// Returns the size of the matrix, in the format {columns, rows}
Matrix::Size Matrix::size() const {
    return {columns, rows};
}

// Returns the volume of the matrix
unsigned int Matrix::volume() const {
    return columns * rows;
}

// Returns the index corresponding to a row-major order offset
Matrix::Index Matrix::index(const unsigned int &offset) const {
    unsigned int column = offset / columns;
    unsigned int row = offset % rows;
    return {column, row};
}

// Returns the row=major order offset of a given column, row index
unsigned int Matrix::offset(const unsigned int &column,
        const unsigned int &row) const {

    return offset({column, row});
}

// Returns the row-major order offset of a given index
unsigned int Matrix::offset(const Index &index) const {
    return index[0] * columns + index[1];
}

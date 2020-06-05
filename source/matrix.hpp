#pragma once

#include <array>
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <vector>

#include "complex.hpp"
#include "log.hpp"

/* ******************************************************************** Synopsis

The Matrix class is used within the source for voltage calculations within the
Simulation class. It represents a classic two-dimensional matrix, which uses
row-major order indexing, and stores each value as an instance of the Complex
class. This was originally to support complex impedances of components, but on
reflection it became clear this functionality was no longer needed. This aspect
of the class will likely be removed, if time allows for it.

The source file is split up into 3 parts:

    (1) Definition
        Outline of the Matrix class and its methods

    (2) Operators
        Arithmetic and equivalence operator overrides

    (3) Constructors

    (4) Size management helpers
        Allow restructuring of the matrix's size, adding/removing columns and
        rows, or re-setting the matrix's values

    (5) Advanced matrix functions
        Helps with complex operations like calculating the derivative, cofactor,
        adjugant, etc.

    (6) Characteristic access methods
        Used for getting the matrix's width, height, volume etc.

    (7) Index/offset helpers
        Convert between row-major order indices, and integer offsets

*/

// ****************************************************************** Definition

class Matrix {

public:

    typedef std::array<unsigned int, 2> Size;
    typedef std::array<unsigned int, 2> Index;

    Complex &operator()(const unsigned int &column, const unsigned int &row);
    Complex operator()(const unsigned int &column, const unsigned int &row)
            const;
    Matrix operator()(const unsigned int &column_one,
            const unsigned int &row_one, const unsigned int &column_two,
            const unsigned int &row_two) const;

    Matrix &operator-=(const Matrix &matrix);
    Matrix &operator*=(const double &factor);
    Matrix &operator*=(const Matrix &matrix);
    Matrix &operator/=(const double &factor);
    Matrix &operator+=(const Matrix &matrix);

    friend bool operator==(const Matrix &one, const Matrix &two);
    friend bool operator-=(const Matrix &one, const Matrix &two);

    Matrix();
    Matrix(const Complex &value);
    Matrix(const std::initializer_list<std::vector<Complex>> &values);
    Matrix(const unsigned int &rows, const unsigned int &columns);

    Matrix &remove_column(const unsigned int &column);
    Matrix &remove_row(const unsigned int &row);
    void clear();
    void resize(const unsigned int &rows, const unsigned int &columns);

    Complex determinant() const;
    Matrix adjugate() const;
    Matrix cofactor() const;
    Matrix inverse() const;
    Matrix transpose() const;

    Size size() const;
    unsigned int columns() const;
    unsigned int rows() const;
    unsigned int volume() const;

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
        const Matrix::Index &index);

bool operator!=(const Matrix &one, const Matrix &two);
bool operator==(const Matrix &one, const Matrix &two);

Matrix operator-(const Matrix &one, const Matrix &two);
Matrix operator*(const Matrix &one, const double &factor);
Matrix operator*(const Matrix &one, const Matrix &two);
Matrix operator/(const Matrix &one, const double &factor);
Matrix operator+(const Matrix &one, const Matrix &two);

/* ******************************************************************* Operators

The operators are split into a few distinct functions:

    a) Stream print operators

    b) Equivalence operators

    c) Value access operators
        Used for getting a values at a specific index within the matrix, or
        submatrices between two indices

    d) Arithmetic operators

*/

/* Serializes a Matrix instance to a stream, in the following form:

    [00, 01, ..., 0n]
    [10, 11, ..., 1n]
    [20, 21, ..., 2n]
    [        ...    ]
    [m0, m1, ..., mn]

*/
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

// Serializes a Matrix::Index to a stream, in the form: (row, column)
std::ostream &operator<<(std::ostream &stream,
        const Matrix::Index &index) {

    return stream << "(" << index[0] << ", " << index[1] << ")";
}

// True if the two matrices aren't equal in size and values
bool operator!=(const Matrix &one, const Matrix &two) {
    return (one == two) == false;
}

// True if the two matrices are equal in size and values
bool operator==(const Matrix &one, const Matrix &two) {
    if(one.size() != two.size())
        return false;

    const auto one_values = one._values;
    const auto two_values = two._values;
    for(unsigned int index = 0; index < one.volume(); index += 1) {
        if(one_values[index] != two_values[index])
            return false;
    }

    return true;
}

// Returns the value at a given column and row index
Complex &Matrix::operator()(const unsigned int &column,
        const unsigned int &row) {

    return _values[offset(row, column)];
}

// Retusn a copy of the value at a given column and row index
Complex Matrix::operator()(const unsigned int &column, const unsigned int &row)
        const {

    return _values[offset(row, column)];
}

// Returns a submatrix between two indices, one and two
Matrix Matrix::operator()(const unsigned int &column_one,
        const unsigned int &row_one, const unsigned int &column_two,
        const unsigned int &row_two) const {

    // Check the left-uppermost corner of this submatrix doesn't extend
    // underneath or to the right of the right-lowermost corner
    if(column_one > column_two || row_one > row_two) {
        Log::error() << "Can't get submatrix with corners (" << row_one <<
                ", " << column_one << "), (" << row_two << ", " << column_two <<
                ")" << std::endl;
        throw -1;
    }

    // Check the submatrix bounds don't extend beyond the size of this matrix
    else if(column_two > _columns || row_two > _rows) {
        Log::error() << "Can't get submatrix with rightmost edge (" <<
                row_two << ", " << column_two << ") from a matrix with size " <<
                size() << std::endl;
        throw -1;
    }

    // Calculate the width of the new matrix
    unsigned int width = column_two - column_one + 1;
    unsigned int height = row_two - row_one + 1;

    // Copy the relevant values to the new submatrix
    Matrix result(width, height);
    for(unsigned int row = 0; row < width; row += 1) {
        for(unsigned int column = 0; column < height; column += 1)
            result(row, column) = (*this)(row_one + row, column_one + column);
    }

    return result;
}

// Subtracts two matrices
Matrix operator-(const Matrix &one, const Matrix &two) {
    return Matrix(one) -= two;
}

// Multiplies each value in a matrix by a factor
Matrix operator*(const Matrix &matrix, const double &factor) {
    return Matrix(matrix) *= factor;
}

// Multiplies two matrices by one another
Matrix operator*(const Matrix &one, const Matrix &two) {
    return Matrix(one) *= two;
}

// Divides each value in a matrix by a scalar factor
Matrix operator/(const Matrix &matrix, const double &factor) {
    return Matrix(matrix) /= factor;
}

// Adds two matrices together
Matrix operator+(const Matrix &one, const Matrix &two) {
    return Matrix(one) += two;
}

// Subtracts a matrix from this instance
Matrix &Matrix::operator-=(const Matrix &matrix) {
    if(this->size() != matrix.size()) {
        Log::error() << "Can't add matrices of sizes " << size() << " and " <<
                matrix.size() << std::endl;
        throw -1;
    }

    for(unsigned int index = 0; index < volume(); index += 1)
        _values[index] -= matrix._values[index];
    return *this;
}

// Multiplies each value in this instance by a factor
Matrix &Matrix::operator*=(const double &factor) {
    for(auto &value : _values)
        value *= factor;
    return *this;
}

// Multiplies this instance by a matrix
Matrix &Matrix::operator*=(const Matrix &matrix) {
    if(_columns != matrix.rows()) {
        Log::error() << "Can't multiply matrices of sizes " << size() <<
                " and " << matrix.size() << std::endl;
        throw -1;
    }

    Matrix result(_rows, matrix.columns());
    for(unsigned int row = 0; row < _rows; row += 1) {
        for(unsigned int column = 0; column < matrix.columns(); column += 1) {
            Complex sum = 0;
            for(unsigned int index = 0; index < _columns; index += 1)
                sum += (*this)(row, index) * matrix(index, column);
            result(row, column) = sum;
        }
    }

    *this = result;
    return *this;
}

// Divides each value in this instance by a scalar factor
Matrix &Matrix::operator/=(const double &factor) {
    for(auto &value : _values)
        value /= factor;
    return *this;
}

// Adds a matrix to this instance
Matrix &Matrix::operator+=(const Matrix &matrix) {
    if(this->size() != matrix.size()) {
        Log::error() << "Can't add matrices of sizes " << size() << " and " <<
                matrix.size() << std::endl;
        throw -1;
    }

    for(unsigned int index = 0; index < volume(); index += 1)
        _values[index] += matrix._values[index];
    return *this;
}

// **************************************************************** Constructors

Matrix::Matrix() {
    _columns = 0;
    _rows = 0;
}

Matrix::Matrix(const Complex &value) {
    resize(1, 1);
    _values[0] = value;
}

Matrix::Matrix(const std::initializer_list<std::vector<Complex>> &values) {

    // Evaluate the number of rows and columns (it's permitted to leave some
    // columns blank, the number of columns in the largest row is taken to be
    // the width of the matrix)
    unsigned int rows = values.size();
    unsigned int columns = 0;
    for(const auto &row : values)
        columns = std::max(row.size(), columns);
    resize(columns, rows);

    // Fill the matrix with the relevant values
    unsigned int row_index = 0;
    unsigned int column_index = 0;
    for(const auto &row : values) {
        column_index = 0;
        for(const auto &value : row) {
            (*this)(row_index, column_index) = value;
            column_index += 1;
        }
        row_index += 1;
    }
}

Matrix::Matrix(const unsigned int &rows, const unsigned int &columns) {
    resize(columns, rows);
}

// ***************************************************** Size management helpers

// Removes a single column from a matrix
Matrix &Matrix::remove_column(const unsigned int &column) {

    // Check the column index provided actually lies within the matrix
    if(column >= _columns) {
        Log::error() << "Can't remove column " << column << " from a matrix of "
                "size " << size() << std::endl;
        throw -1;
    }

    // Create a new temporary matrix of the reduced width
    Matrix result(_columns - 1, _rows);

    // Copy over the old values to the new matrix, skipping the column which is
    // meant to be deleted
    for(unsigned int row_index = 0; row_index < _rows; row_index += 1) {
        unsigned int offset = 0;
        for(unsigned int column_index = 0; column_index < (_columns - 1);
                column_index += 1) {

            if(column_index == column)
                offset += 1;

            result(row_index, column_index) =
                    (*this)(row_index, column_index + offset);
        }
    }

    // Re-assign this matrix, and return its reference
    *this = result;
    return *this;
}

// Removes a single row from a matrix
Matrix &Matrix::remove_row(const unsigned int &row) {

    // Check the column index provided actually lies within the matrix
    if(row >= _rows) {
        Log::error() << "Can't remove row " << row << " from a matrix of "
                "size " << size() << std::endl;
        throw -1;
    }

    // Create a new temporary matrix of the reduced height
    Matrix result(_columns, _rows - 1);

    // Copy over the old values to the new matrix, skipping the row which is
    // meant to be deleted
    unsigned int offset = 0;
    for(unsigned int row_index = 0; row_index < (_rows - 1); row_index += 1) {
        if(row_index == row)
            offset += 1;

        for(unsigned int column_index = 0; column_index < _columns;
                column_index += 1) {
            result(row_index, column_index) =
                    (*this)(row_index + offset, column_index);
        }
    }

    // Re-assign this matrix, and return its reference
    *this = result;
    return *this;
}

// Clears (sets to zero) each value in a matrix
void Matrix::clear() {
    for(auto &value : _values)
        value = 0;
}

// Resizes a matrix, copying over the values whose indices overlap between the
// old, and the resized matrix
void Matrix::resize(const unsigned int &rows, const unsigned int &columns) {

    // If the resize function is called by the constructor, then the values
    // field will be empty. This being the case, the matrix's old values don't
    // need to be stored in the new matrix
    if(_values.empty()) {
        _rows = rows;
        _columns = columns;
        _values.resize(rows * columns, 0);
        return;
    }

    // Create a new temporary matrix
    Matrix temporary(*this);

    // Resize the matrix's member variables to reflect its new size, and clear
    // the old values (these have been stored in the temporary matrix)
    _rows = rows;
    _columns = columns;
    _values.resize(rows * columns, 0);
    _values.clear();

    // Copy over the values from the old temporary matrix, if they overlap with
    // the new matrix
    auto row_bound = std::min(_rows, temporary.rows());
    auto column_bound = std::min(_columns, temporary.columns());
    for(unsigned int row = 0; row < row_bound; row += 1) {
        for(unsigned int column = 0; column < column_bound; column += 1)
            (*this)(row, column) = temporary(row, column);
    }
}

// *************************************************** Advanced matrix functions

// Calculates the matrix's determinant
Complex Matrix::determinant() const {

    // Only square matrices can have their determinants evaluated
    if(_columns != _rows) {
        Log::error() << "Can't get the determinant of a non-square matrix " <<
                size() << std::endl;
        throw -1;
    }

    auto size = _columns;

    // Create two matrices to hold the lower and upper decompositions
    Matrix lower_matrix(size, size);
    Matrix upper_matrix(size, size);

    // I found this algorithm on StackOverflow somewhere, but it works...
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

    // Calculate the determinant of the matrix as described in the function
    // description
    Complex result = 1;
    for(unsigned int offset = 0; offset < size; offset += 1) {
        result *= lower_matrix(offset, offset);
        result *= upper_matrix(offset, offset);
    }

    // Without this step, a determinant close to zero might be evaluated as
    // '-0'. This isn't strictly-speaking a mathematical problem, but it did
    // mean that printing the value would look strange, so I've added this
    // section to round values within the double data type's epsilon range to 0.
    const auto epsilon = std::numeric_limits<double>::epsilon();
    result.real_part = (std::fabs(result.real_part - 0.0) >= epsilon) ?
            result.real_part : 0;
    result.imaginary_part = (std::fabs(result.imaginary_part - 0.0) >=
            epsilon) ? result.imaginary_part : 0;
    return result;
}

// Calculates the matrix's adjugant
Matrix Matrix::adjugate() const {
    return cofactor().transpose();
}

// Calculates the matrix's cofactor
Matrix Matrix::cofactor() const {

    // Check the matrix is square
    if(_columns != _rows) {
        Log::error() << "Can't create cofactor matrix of a non-square "
                "matrix, of size " << size() << std::endl;
        throw -1;
    }

    // Create a result matrix
    const auto size = _columns;
    Matrix result(size, size);

    // Apply the steps outlined in this function's description
    for(unsigned int column = 0; column < _columns; column += 1) {
        for(unsigned int row = 0; row < _rows; row += 1) {
            Matrix submatrix = *this;
            submatrix.remove_column(column);
            submatrix.remove_row(row);

            int sign = (((column + row) % 2) == 0) ? 1 : -1;
            result(row, column) = submatrix.determinant() * sign;
        }
    }

    return result;
}

// Calculates the matrix's inverse
Matrix Matrix::inverse() const {

    // Check the matrix is square
    if(_rows != _columns) {
        Log::error() << "Can't create inverse matrix of a non-square matrix " <<
                size() << std::endl;
        throw -1;
    }

    // Calculate the determinant, and check it's non-zero (this would cause a
    // division-by-zero error)
    auto _determinant = this->determinant();
    if(_determinant == 0) {
        Log::error() << "Can't get inverse of matrix with a zero determinant" <<
                std::endl;
        throw -1;
    }

    // Calculate its adjugate, and apply the operation outlined in this
    // function's description
    auto _adjugate = this->adjugate();
    auto size = _columns;
    Matrix result(size, size);
    for(unsigned int row = 0; row < size; row += 1) {
        for(unsigned int column = 0; column < size; column += 1)
            result(row, column) = _adjugate(row, column) / _determinant;
    }

    return result;
}

// Calculates the matrix's transpose
Matrix Matrix::transpose() const {
    Matrix result(_rows, _columns);
    for(unsigned int column = 0; column < _columns; column += 1) {
        for(unsigned int row = 0; row < _rows; row += 1)
            result(column, row) = (*this)(row, column);
    }
    return result;
}

// *********************************************** Characteristic access methods

// Retusn a Matrix::Size instance, containing the values {width, height}
Matrix::Size Matrix::size() const {
    return {_rows, _columns};
}

// Returns the number of columns in the matrix (its width)
unsigned int Matrix::columns() const {
    return _columns;
}

// Returns the number of rows in the matrix (its height)
unsigned int Matrix::rows() const {
    return _rows;
}

// Returns the volume of the matrix (columns * rows)
unsigned int Matrix::volume() const {
    return _columns * _rows;
}

// ******************************************************** Index/offset helpers

// Returns the index representing a row-major order offset
Matrix::Index Matrix::index(const unsigned int &offset) const {
    return Index({offset / _rows, offset % _columns});
}

// Returns the row-major offset corresponding to an index
unsigned int Matrix::offset(const Index &index) const {
    return offset(index[0], index[1]);
}

// Returns the row-major offset corresponding to an index
unsigned int Matrix::offset(const unsigned int &row, const unsigned int &column)
        const {

    unsigned int result = (row * _columns) + column;
    if(result > (_columns * _rows)) {
        Log::error() << "Can't access element at (" << row << ", " << column <<
                ") from a matrix of size " << size() << " (offset = " <<
                result << ")" << std::endl;
        throw -1;
    }
    return result;
}

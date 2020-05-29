#pragma once

#include <array>
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

/*
Prints an matrix to a stream, in the following format:
    [ 00, 01, ..., 0n]
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

// Prints an index to a stream, in the format: (row, column)
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
    return Matrix(one) += two;
}

// Returns true if the matrices are equal
// NOTE: This relies on the Complex class equality operator overload. If
// you want to evaluate equality between the real or imaginary parts of two
// matrices, make sure to factor in the double epsilon from std::numeric_limits
bool operator==(const Matrix &one, const Matrix &two) {
    if(one.size() != two.size())
        return false;

    const auto one_values = one.values();
    const auto two_values = two.values();
    auto epsilon = std::numeric_limits<double>::epsilon();
    for(unsigned int index = 0; index < one.volume(); index += 1) {
        if(one_values[index] != two_values[index])
            return false;
    }

    return true;
}

// Returns true if the two matrices aren't equal
bool operator!=(const Matrix &one, const Matrix &two) {
    return (one == two) == false;
}

// Returns the identity matrix of a given size
// This is a matrix whose elements along the diagonal are 1, and all others are
// 0
Matrix Matrix::identity(const unsigned int &size) {
    Matrix matrix(size, size);
    for(unsigned int index = 0; index < size; index += 1)
        matrix(index, index) = 1;
    return matrix;
}

// Returns the value at a given row and index
// NOTE: The row field comes first, since this class uses row-major order
Complex &Matrix::operator()(const unsigned int &row,
        const unsigned int &column) {

    return _values[offset(row, column)];
}

// Returns the value at a given column and row index
// NOTE: The row field comes first, since this class uses row-major order
Complex Matrix::operator()(const unsigned int &row, const unsigned int &column)
        const {

    return _values[offset(row, column)];
}

// Returns a submatrix
Matrix Matrix::operator()(const unsigned int &column_one,
        const unsigned int &row_one, const unsigned int &column_two,
        const unsigned int &row_two) const {

    // Check the left-uppermost corner of this submatrix doesn't extend
    // underneath or to the right of the right-lowermost corner
    if(column_one > column_two || row_one > row_two) {
        std::cerr << "Can't get submatrix with corners (" << row_one << ", " <<
                column_one << "), (" << row_two << ", " << column_two << ")" <<
                std::endl;
        throw -1;
    }

    // Check the submatrix bounds don't extend beyond the size of this matrix
    else if(column_two > _columns || row_two > _rows) {
        std::cerr << "Can't get submatrix with rightmost edge (" << row_two <<
                ", " << column_two << ") from a matrix with size " << size() <<
                std::endl;
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

// Multiplies the matrix by a scalar factor
Matrix &Matrix::operator*=(const double &factor) {
    for(auto &value : _values)
        value *= factor;
    return *this;
}

// Multiplies the matrix by another matrix
Matrix &Matrix::operator*=(const Matrix &matrix) {
    if(_columns != matrix.rows()) {
        std::cout << "Can't multiply matrices of sizes " << size() << " and " <<
                matrix.size();
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

// Divides the matrix by a scalar factor
Matrix &Matrix::operator/=(const double &factor) {
    for(auto &value : _values)
        value /= factor;
    return *this;
}

// Adds a matrix to this matrix
Matrix &Matrix::operator+=(const Matrix &matrix) {
    if(this->size() != matrix.size()) {
        std::cout << "Can't add matrices of sizes " << size() << " and " <<
                matrix.size();
        throw -1;
    }

    const auto values = matrix.values();
    for(unsigned int index = 0; index < volume(); index += 1)
        _values[index] += values[index];
    return *this;
}

Matrix::Matrix() {
    _columns = 0;
    _rows = 0;
}

Matrix::Matrix(const Complex &value) {
    resize(1, 1);
    values[0] = value;
}

// Construct a matrix from a vector of vector of complex numbers (where the
// first vector contains the rows, and the second vector, the values in each
// column)
Matrix::Matrix(const std::vector<std::vector<Complex>> &values) {

    // Evaluate the number of rows and columns (it's permitted to leave some
    // columns blank, the number of columns in the largest row is taken to be
    // the width of the matrix)
    unsigned int rows = values.size();
    unsigned int columns = 0;
    for(const auto &row : values)
        columns = std::max(row.size(), columns);
    resize(columns, rows);

    // Fill the matrix with the relevant values
    for(unsigned int row = 0; row < rows; row += 1) {
        for(unsigned int column = 0; column < columns; column += 1)
            (*this)(row, column) = values[row][column];
    }
}

// Construct a matrix from an initializer list, in much the same way as the
// nested vector constructor
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

Matrix::Matrix(const unsigned int &columns, const unsigned int &rows) {
    resize(columns, rows);
}

// Resizes the matrix, copying any overlapping data into the resized matrix
void Matrix::resize(const unsigned int &columns, const unsigned int &rows) {

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

// Clear the values in the matrix, leaving its size intact
void Matrix::clear() {
    for(auto &value : _values)
        value = 0;
}

// Removes a single column from the matrix
Matrix &Matrix::remove_column(const unsigned int &column) {

    // Check the column index provided actually lies within the matrix
    if(column >= _columns) {
        std::cerr << "Can't remove column " << column << " from a matrix of "
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

// Removes a row from the matrix
Matrix &Matrix::remove_row(const unsigned int &row) {

    // Check the column index provided actually lies within the matrix
    if(row >= _rows) {
        std::cerr << "Can't remove row " << row << " from a matrix of size " <<
                size() << std::endl;
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

/*
Returns the determinant of the matrix

To calculate the determinant of a matrix, first, LU (lower-upper) decomposition
is performed. This process creates two triangular matrices, the lower and
upper.

Since the determinant of this matrix is the product of the determinants of the
lower and upper matrices, and the determinant of a triangular matrix is the
product of each element along its diagonal, the determinant of this matrix can
be evaluated
*/
Complex Matrix::determinant() const {

    // Only square matrices can have their determinants evaluated
    if(_columns != _rows) {
        std::cerr << "Can't get the determinant of a non-square matrix " <<
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

// Returns the adjugate of the matrix (the transpose of the matrix's cofactor)
Matrix Matrix::adjugate() const {
    return cofactor().transpose();
}

/*
Returns the cofactor of the matrix

Steps in creating a cofactor matrix:
    (1) For each row and column in the matrix, create a sumbatrix with the
        relevant row and column removed. The new submatrix should be 1 smaller
        in both width and height
    (2) Take the determinant of this new submatrix, and assign it to the
        relevant row and column in the resultant matrix
    (3) Apply a checkerboard pattern of negation to the result matrix. In short,
        any element which has an even row and column index should be negated
*/
Matrix Matrix::cofactor() const {

    // Check the matrix is square
    if(_columns != _rows) {
        std::cerr << "Can't create cofactor matrix of a non-square matrix " <<
                size() << std::endl;
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

/*
Returns the inverse of the matrix

To find a matrix's inverse, set each element of the result to the value of the
matrix's adjugate counterpart, dividing it by this matrix's determinant
*/
Matrix Matrix::inverse() const {

    // Check the matrix is square
    if(_rows != _columns) {
        std::cerr << "Can't create inverse matrix of a non-square matrix " <<
                size() << std::endl;
        throw -1;
    }

    // Calculate the determinant, and check it's non-zero (this would cause a
    // division-by-zero error)
    auto _determinant = this->determinant();
    if(_determinant == 0) {
        std::cerr << "Can't get inverse of matrix with a zero determinant" <<
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

// Returns the transpose of the matrix (a matrix with the rows and columns
// swapped)
Matrix Matrix::transpose() const {
    Matrix result(_rows, _columns);
    for(unsigned int column = 0; column < _columns; column += 1) {
        for(unsigned int row = 0; row < _rows; row += 1)
            result(column, row) = (*this)(row, column);
    }
    return result;
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
    return {_rows, _columns};
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

    unsigned int result = (row * _columns) + column;
    if(result > (_columns * _rows)) {
        std::cerr << "Can't access element at (" << row << ", " << column <<
                ") from a matrix of size " << size() << " (offset = " <<
                result << ")" << std::endl;
        throw -1;
    }
    return result;
}

// Returns the row-major order offset of a given index
unsigned int Matrix::offset(const Index &index) const {
    return offset(index[0], index[1]);
}

// Returns a reference the raw values contained by the matrix
std::vector<Complex> &Matrix::values() {
    return _values;
}

// Returns the raw values contained by the matrix
std::vector<Complex> Matrix::values() const {
    return _values;
}

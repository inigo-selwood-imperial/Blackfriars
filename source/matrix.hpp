#pragma once

#include <array>
#include <limits>
#include <ostream>
#include <vector>

#include <cmath>

#include "complex.hpp"

class Matrix {

public:

    typedef std::array<unsigned int, 2> Size;
    typedef std::array<unsigned int, 2> Index;

    static Matrix identity(const unsigned int &size);

    static Matrix power(const Matrix &matrix, const unsigned int &exponent);

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
    return Matrix(one) += two;
}

// Returns true if the matrices are equal
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
Complex &Matrix::operator()(const unsigned int &row,
        const unsigned int &column) {

    return _values[offset(row, column)];
}

// Returns the value at a given column and row index
Complex Matrix::operator()(const unsigned int &row, const unsigned int &column)
        const {

    return _values[offset(row, column)];
}

// Returns a submatrix
Matrix Matrix::operator()(const unsigned int &column_one,
        const unsigned int &row_one, const unsigned int &column_two,
        const unsigned int &row_two) const {

    if(column_one > column_two || row_one > row_two) {
        std::cerr << "Can't get submatrix with corners (" << row_one << ", " <<
                column_one << "), (" << row_two << ", " << column_two << ")" <<
                std::endl;
        throw -1;
    }
    else if(column_two > _columns || row_two > _rows) {
        std::cerr << "Can't get submatrix with rightmost edge (" << row_two <<
                ", " << column_two << ") from a matrix with size " << size() <<
                std::endl;
        throw -1;
    }

    unsigned int width = column_two - column_one + 1;
    unsigned int height = row_two - row_one + 1;

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

Matrix::Matrix(const std::vector<std::vector<Complex>> &values) {
    unsigned int rows = values.size();
    unsigned int columns = 0;
    for(const auto &row : values)
        columns = std::max(row.size(), columns);
    resize(columns, rows);

    for(unsigned int row = 0; row < rows; row += 1) {
        for(unsigned int column = 0; column < columns; column += 1)
            (*this)(row, column) = values[row][column];
    }
}

Matrix::Matrix(const std::initializer_list<std::vector<Complex>> &values) {
    unsigned int rows = values.size();
    unsigned int columns = 0;
    for(const auto &row : values)
        columns = std::max(row.size(), columns);
    resize(columns, rows);

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

// Removes a single column from the matrix
Matrix &Matrix::remove_column(const unsigned int &column) {
    if(column >= _columns) {
        std::cerr << "Can't remove column " << column << " from a matrix of "
                "size " << size() << std::endl;
        throw -1;
    }

    Matrix result(_columns - 1, _rows);

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

    *this = result;
    return *this;
}

// Removes a row from the matrix
Matrix &Matrix::remove_row(const unsigned int &row) {
    if(row >= _rows) {
        std::cerr << "Can't remove row " << row << " from a matrix of size " <<
                size() << std::endl;
        throw -1;
    }

    Matrix result(_columns, _rows - 1);

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

    *this = result;
    return *this;
}

// Returns the determinant of the matrix
Complex Matrix::determinant() const {
    if(_columns != _rows) {
        std::cerr << "Can't get the determinant of a non-square matrix " <<
                size() << std::endl;
        throw -1;
    }

    auto size = _columns;

    Matrix lower_matrix(size, size);
    Matrix upper_matrix(size, size);

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

    Complex result = 1;
    for(unsigned int offset = 0; offset < size; offset += 1) {
        result *= lower_matrix(offset, offset);
        result *= upper_matrix(offset, offset);
    }

    const auto epsilon = std::numeric_limits<double>::epsilon();
    result.real_part = (std::fabs(result.real_part - 0.0) >= epsilon) ?
            result.real_part : 0;
    result.imaginary_part = (std::fabs(result.imaginary_part - 0.0) >=
            epsilon) ? result.imaginary_part : 0;
    return result;
}

// Returns the adjugate of the matrix
Matrix Matrix::adjugate() const {
    return cofactor().transpose();
}

// Returns the cofactor of the matrix
Matrix Matrix::cofactor() const {
    if(_columns != _rows) {
        std::cerr << "Can't create cofactor matrix of a non-square matrix " <<
                size() << std::endl;
        throw -1;
    }

    const auto size = _columns;
    Matrix result(size, size);

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

// Returns the inverse of the matrix
Matrix Matrix::inverse() const {
    if(_rows != _columns) {
        std::cerr << "Can't create inverse matrix of a non-square matrix " <<
                size() << std::endl;
        throw -1;
    }

    auto _determinant = this->determinant();
    if(_determinant == 0) {
        std::cerr << "Can't get inverse of matrix with a zero determinant" <<
                std::endl;
        throw -1;
    }

    auto _adjugate = this->adjugate();
    auto size = _columns;
    Matrix result(size, size);
    for(unsigned int row = 0; row < size; row += 1) {
        for(unsigned int column = 0; column < size; column += 1)
            result(row, column) = _adjugate(row, column) / _determinant;
    }

    return result;
}

// Returns the transpose of the matrix
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

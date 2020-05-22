#pragma once

// TODO: Implement a custom iterator
class Matrix {

private:

    std::vector<std::vector<double>> values;

public:

    static Matrix identity_matrix(const unsigned int &size);

    static double dot_product(const Matrix &one, const Matrix &two);
    static Matrix cross_product(const Matrix &one, const Matrix &two);

    static Matrix operator*(const Matrix &matrix, const double &factor);
    static Matrix operator/(const Matrix &matrix, const double &factor);
    static Matrix operator+(const Matrix &one, const Matrix &two);

    static bool operator!=(const Matrix &one, const Matrix &two);
    static bool operator==(const Matrix &one, const Matrix &two);

    double &operator()(const unsigned int &column, const unsigned int &row);

    Matrix &operator*=(const double &factor);
    Matrix &operator/=(const double &factor);
    Matrix &operator+=(const Matrix &matrix);

    Matrix();
    Matrix(const std::vector<std::vector<double>> &values);
    Matrix(const unsigned int &width, const unsigned int &height);

    void resize(const unsigned int &width, const unsigned int &height);
    void clear();

    double derivative() const;
    Matrix inverse() const;
    Matrix transpose() const;

    unsigned int height() const;
    unsigned int width() const;

    std::array<unsigned int, 2> size();
    unsigned int volume() const;

};

// Prints an matrix to a stream
std::ostream &operator<<(std::ostream &stream, const Matrix &matrix) {

}

// Prints an index to a stream
std::ostream &operator<<(std::ostream &stream, const std::array<unsigned int, 2> &index) {
    return stream << "(" << index[0] << ", " << index[1] << ")";
}

// Returns the identity matrix of a given size
Matrix Matrix::identity_matrix(const unsigned int &size) {
    Matrix matrix(size, size);
    for(unsigned int index = 0; index < size; index += 1)
        matrix(index, index) = 1;
    return matrix;
}

// Returns the dot product of two matrices
double Matrix::dot_product(const Matrix &one, const Matrix &two) {

}

// Returns the cross product of two matrices
Matrix Matrix::cross_product(const Matrix &one, const Matrix &two) {

}

// Returns a matrix multiplied by a scalar factor
Matrix Matrix::operator*(const Matrix &matrix, const double &factor) {
    const auto size = matrix.size();
    Matrix new_matrix = matrix;
    for(unsigned int column = 0; column < size[0]; column += 1) {
        for(unsigned int row = 0; row < size[1]; row += 1)
            new_matrix(column, row) *= factor;
    }
    return new_matrix;
}

// Returns a matrix divided by a scalar factor
Matrix Matrix::operator/(const Matrix &matrix, const double &factor) {
    return matrix * (1 / factor);
}

// Returns the sum of two matrices
Matrix Matrix::operator+(const Matrix &one, const Matrix &two) {
    if(one.size() != two.size()) {
        std::cerr << "Can't add two matrices of different sizes" << std::endl;
        throw -1;
    }

    const auto size = one.size();
    Matrix new_matrix = one;
    for(unsigned int column = 0; column < size[0]; column += 1) {
        for(unsigned int row = 0; row < size[1]; row += 1)
            new_matrix(column, row) += two(column, row);
    }
    return new_matrix;
}

// Returns true if the two matrices aren't equal
bool Matrix::operator!=(const Matrix &one, const Matrix &two) {
    return (one == two) == false;
}

// Returns true if the matrices are equal
bool Matrix::operator==(const Matrix &one, const Matrix &two) {
    if(one.size() != two.size())
        return false;

    const auto epsilon = std::numeric_limits<double>::epsilon();
    for(unsigned int column = 0; column < size[0]; column += 1) {
        for(unsigned int row = 0; row < size[1]; row += 1) {
            if(std::fabs(one(column, row) - two(column, row)) >= epsilon)
                return false;
        }
    }

    return true;
}

// Returns the value at a given column and row index
double &Matrix::operator()(const unsigned int &column,
        const unsigned int &row) {

    if(column >= values.size() || row >= values[column].size())
        return 0;


}

// Multiplies the matrix by a scalar factor
Matrix &Matrix::operator*=(const double &factor) {

}

// Divides the matrix by a scalar factor
Matrix &Matrix::operator/=(const double &factor) {

}

// Adds a matrix to this matrix
Matrix &Matrix::operator+=(const Matrix &matrix) {

}

Matrix::Matrix() {

}

Matrix::Matrix(const std::vector<std::vector<double>> &values) {

}

Matrix::Matrix(const unsigned int &width, const unsigned int &height) {

}

// Resizes the matrix, copying any overlapping data into the resized matrix
void Matrix::resize(const unsigned int &width, const unsigned int &height) {

}

// Clear the values in the matrix, leaving its size intact
void Matrix::clear() {

}

// Returns the derivative of the matrix
double Matrix::derivative() const {

}

// Returns the inverse of the matrix
Matrix Matrix::inverse() const {

}

// Returns the transpose of the matrix
Matrix Matrix::transpose() const {

}

// Returns the height of the matrix
unsigned int Matrix::height() const {

}

// Returns the width of the matrix
unsigned int Matrix::width() const {

}

// Returns the size of the matrix, in the format {width, height}
std::array<unsigned int, 2> Matrix::size() {

}

// Returns the volume of the matrix (width * height)
unsigned int Matrix::volume() const {

}

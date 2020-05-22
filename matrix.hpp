#pragma once

// TODO: Implement a custom iterator
class Matrix {

public:

    typedef std::array<unsigned int, 2> Size;
    typedef std::array<unsigned int, 2> Index;

    static Matrix identity_matrix(const unsigned int &size);

    static double dot_product(const Matrix &one, const Matrix &two);
    static Matrix cross_product(const Matrix &one, const Matrix &two);
    static Matrix power(const Matrix &matrix, const unsigned int &exponent);

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

    Size size();
    unsigned int volume() const;

private:

    std::vector<double> values;

    static Index index(const unsigned int &offset);
    static unsigned int offset(const Index &index);

};

// Prints an matrix to a stream
std::ostream &operator<<(std::ostream &stream, const Matrix &matrix) {}

// Prints an index to a stream
std::ostream &operator<<(std::ostream &stream, const std::array<unsigned int, 2> &index) {}

// Returns the identity matrix of a given size
Matrix Matrix::identity_matrix(const unsigned int &size) {}

// Returns the dot product of two matrices
double Matrix::dot_product(const Matrix &one, const Matrix &two) {}

// Returns the cross product of two matrices
Matrix Matrix::cross_product(const Matrix &one, const Matrix &two) {}

// Returns the result of a matrix to a given power
Matrix Matrix::power(const Matrix &matrix, const unsigned int &exponent);

// Returns a matrix multiplied by a scalar factor
Matrix Matrix::operator*(const Matrix &matrix, const double &factor) {}

// Returns a matrix divided by a scalar factor
Matrix Matrix::operator/(const Matrix &matrix, const double &factor) {}

// Returns the sum of two matrices
Matrix Matrix::operator+(const Matrix &one, const Matrix &two) {}

// Returns true if the two matrices aren't equal
bool Matrix::operator!=(const Matrix &one, const Matrix &two) {}

// Returns true if the matrices are equal
bool Matrix::operator==(const Matrix &one, const Matrix &two) {}

// Returns the value at a given column and row index
double &Matrix::operator()(const unsigned int &column,
        const unsigned int &row) {}

// Multiplies the matrix by a scalar factor
Matrix &Matrix::operator*=(const double &factor) {}

// Divides the matrix by a scalar factor
Matrix &Matrix::operator/=(const double &factor) {}

// Adds a matrix to this matrix
Matrix &Matrix::operator+=(const Matrix &matrix) {}

Matrix::Matrix() {}

Matrix::Matrix(const std::vector<std::vector<double>> &values) {}

Matrix::Matrix(const unsigned int &width, const unsigned int &height) {}

// Resizes the matrix, copying any overlapping data into the resized matrix
void Matrix::resize(const unsigned int &width, const unsigned int &height) {}

// Clear the values in the matrix, leaving its size intact
void Matrix::clear() {}

// Returns the derivative of the matrix
double Matrix::derivative() const {}

// Returns the inverse of the matrix
Matrix Matrix::inverse() const {}

// Returns the transpose of the matrix
Matrix Matrix::transpose() const {}

// Returns the height of the matrix
unsigned int Matrix::height() const {}

// Returns the width of the matrix
unsigned int Matrix::width() const {}

// Returns the size of the matrix, in the format {width, height}
std::array<unsigned int, 2> Matrix::size() {}

// Returns the volume of the matrix (width * height)
unsigned int Matrix::volume() const {}

// Returns the index corresponding to a row-major order offset
Index Matrix::index(const unsigned int &offset) {}

// Returns the row-major order offset of a given index
unsigned int Matrix::offset(const Index &index) {}

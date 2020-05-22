#pragma once

class Matrix {

private:

    std::vector<std::vector<double>> values;

public:

    static Matrix identity_matrix(const unsigned int &dimension);

    static double dot_product(const Matrix &one, const Matrix &two);
    static Matrix cross_product(const Matrix &one, const Matrix &two);

    static Matrix operator*(const Matrix &one, const double &factor);
    static Matrix operator/(const Matrix &one, const double &factor);
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

std::ostream &Matrix::operator<<(std::ostream &stream, const Matrix &matrix) {
    
}

Matrix Matrix::identity_matrix(const unsigned int &dimension) {

}

double Matrix::dot_product(const Matrix &one, const Matrix &two) {

}

Matrix Matrix::cross_product(const Matrix &one, const Matrix &two) {

}

Matrix Matrix::operator*(const Matrix &one, const double &factor) {

}

Matrix Matrix::operator/(const Matrix &one, const double &factor) {

}

Matrix Matrix::operator+(const Matrix &one, const Matrix &two) {

}

bool Matrix::operator!=(const Matrix &one, const Matrix &two) {

}

bool Matrix::operator==(const Matrix &one, const Matrix &two) {

}

double &Matrix::operator()(const unsigned int &column,
        const unsigned int &row) {

}

Matrix &Matrix::operator*=(const double &factor) {

}

Matrix &Matrix::operator/=(const double &factor) {

}

Matrix &Matrix::operator+=(const Matrix &matrix) {

}

Matrix::Matrix() {

}

Matrix::Matrix(const std::vector<std::vector<double>> &values) {

}

Matrix::Matrix(const unsigned int &width, const unsigned int &height) {

}

void Matrix::resize(const unsigned int &width, const unsigned int &height) {

}

void Matrix::clear() {

}

double Matrix::derivative() const {

}

Matrix Matrix::inverse() const {

}

Matrix Matrix::transpose() const {

}

unsigned int Matrix::height() const {

}

unsigned int Matrix::width() const {

}

std::array<unsigned int, 2> Matrix::size() {

}

unsigned int Matrix::volume() const {

}

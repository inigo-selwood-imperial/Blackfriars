#include <iostream>
#include <string>
#include <map>

#include "matrix.hpp"

int main() {
    Matrix matrix = {
        {2, -3, 0},
        {-3, 12, -7},
        {0, -7, 5}
    };
    std::cout << matrix.determinant() << std::endl;
}

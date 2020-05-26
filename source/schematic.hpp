#pragma once

#include "component.hpp"
#include "matrix.hpp"

class Schematic {

private:

    Matrix a_matrix();

    Matrix b_matrix();
    Matrix c_matrix();
    Matrix d_matrix();
    Matrix g_matrix();

    Matrix y_matrix();

    Matrix x_matrix();

    Matrix j_matrix();
    Matrix v_matrix();

    Matrix z_matrix();

public:

    Schematic(const std::string &text);

    Matrix dc_analysis();

    Matrix ac_analysis();

};

Matrix Schematic::a_matrix();

Matrix Schematic::b_matrix();
Matrix Schematic::c_matrix();
Matrix Schematic::d_matrix();
Matrix Schematic::g_matrix();

Matrix Schematic::y_matrix();

Matrix Schematic::x_matrix();

Matrix Schematic::j_matrix();
Matrix Schematic::v_matrix();

Matrix Schematic::z_matrix();

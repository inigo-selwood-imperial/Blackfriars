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

    Matrix e_matrix();
    Matrix i_matrix();

public:

    Schematic(const std::string &text);

};

/*
The A matrix, size (M + N)^2, is a combination of 4 smaller matrices, G, B, C,
    and D (Where N is the no. nodes, and M is the no. independent voltage
    sources)

    A = | G B |
        | C D |

- The G matrix, size N * N, and is filled with nodes
- The B matrix, size N * M, and contains voltage sources
- The C matrix, size M * N, is the transpose of B
- The D matrix, size M * M, is zero when there are no dependent sources
*/
Matrix Schematic::a_matrix() {}

/*
The B matrix (size N * M) has only the values -1, 0, 1. Each location in the
matrix corresponds to a particular voltage source (in the first dimension), or a
node (in the second dimension).

- If the positive terminal of the Jth voltage source is connected to node K,
    then the element (K, J) is 1
- If the negative terminal of the Jth voltage source is connected to node K,
    then the element (K, J) in the matrix is -1
- Otherwise, all elements of the matrix are 0
*/
Matrix Schematic::b_matrix() {}

/*
The C matrix(size M * N) is the transpose of the B matrix
NOTE: This is not the case when dependent sources are present!
*/
Matrix Schematic::c_matrix() {}

/*
The D matrix (size M * M) is just an empty (zero-filled) matrix
NOTE: As with the C matrix, this is not the case when dependent voltage sources
are involved
*/
Matrix Schematic::d_matrix() {}

/* The G matrix (size N * N) is formed in two steps:

(1) Each element along the diagonal of the matrix is the sum of the conductance
    of each element connected to the corresponding node. So, the first diagonal
    element is the sum of conductances connected to node 1, the second is the
    sum of those connected to node 2, etc.

(2) The off-diagonal elements are the negative conductance of the element
    connected to the pair of corresponding nodes. Therefore, a resistor between
    nodes 1 and 2 goes into the G matrix at (1, 2) and (2, 1)
*/
Matrix Schematic::g_matrix() {}

/*
The X matrix (size M + N) holds the unknown quantities, and is a combination of
2 smaller matrices V and J:

    X = | V |
        | J |

- The V matrix (size 1 * N) holds the unknown voltages
- The J matrix (size ! * M) holds the unknown currents through the voltage
    sources
*/
Matrix Schematic::x_matrix() {}

/*
The J matrix (size 1 * M) has one entry for the current through each voltage
source. So, if there are M voltages soources (V_1, V_2, ..., V_M), then the J
matrix would look like:

        | iV_1 |
    J = | iV_2 |
        | ...  |
        | iV_M |
*/
Matrix Schematic::j_matrix() {}

/*
The V matrix (size 1 * N) is formed of the node voltages. Each element in V
corresponds to the voltage at the quivalent node in the circuit.
NOTE: There is no entry for ground (node 0)

For a circuit of N nodes, V would look like:

        | V_1 |
    V = | V_2 |
        | ... |
        | V_N |
*/
Matrix Schematic::v_matrix() {}

/*
The Z matrix (size 1 * (M + N)) holds the independent voltage and current
sources, and is again a combination of 2 smaller matrices (I and E).

    Z = | I |
        | E |

- The I matrix (size 1 * N) contains the sum of the currents through the passive
    elements going into the corresponding node (either zero, or the sum of
    independent current sources)
- The E matrix (size 1 * M) holds the values of the independent voltage sources
*/
Matrix Schematic::z_matrix() {}

/*
The E matrix (size 1 * M) contains the values of the independent voltage sources
*/
Matrix Schematic::e_matrix() {}

/*
The I matrix (size 1 * N) contains the sum of currents through the passive
elements into the corresponding node. If there are no current sources into the
connected node, the value should be zero.
*/
Matrix Schematic::i_matrix() {}

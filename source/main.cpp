#include <iostream>

#include "schematic.hpp"

int main() {
    try {
        auto netlist = Parse::load("..\\resources\\test_1.net");
        Schematic schematic(netlist);
        std::cout << schematic << std::endl;
    }
    catch(...) {
        return -1;
    }
    return 0;
}

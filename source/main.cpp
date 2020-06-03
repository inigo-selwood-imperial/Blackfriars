#include <iostream>

#include "schematic.hpp"

int main() {
    try {
        auto netlist = Parse::load_file("..\\resources\\test_1.net");
        std::cout << "Netlist: " << netlist << std::endl;
        Schematic schematic(netlist);
        std::cout << "Printing schematic" << std::endl << schematic << std::endl;
    }
    catch(...) {
        return -1;
    }
    return 0;
}

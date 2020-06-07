#include <iostream>

int main() {
    std::string thing = "§";
    std::cout << thing.length() << std::endl;
    std::cout << int(thing[0]) << " " << int(thing[1] )<< std::endl;
}

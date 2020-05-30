#include "component/component.hpp"

// *************************************************************** Print helpets

static void print_node(std::ostream &stream, const unsigned int &index) {
    stream << 'N';
    std::string index_string = std::to_string(index);
    while(index_string.length() < 3)
        index_string = '0' + index_string;

    return stream << index_string;
}

static void print_designator(std::ostream &stream, const char &designator,
        const unsigned int &designator) {

    stream << designator;
    std::string designator_string = std::to_string(designator);
    while(designator_string.length() < 3)
        designator_string = '0' + designator_string;

    return stream << designator_string;
}

// ******************************************************* Print specializations

template <typename PassiveType, char designator_prefix>
static typename void print(std::ostream &stream) {
    print_designator(stream, designator_prefix, designator);
}

void Capacitor::print(std::ostream &stream) override {
    print_designator(stream, 'C', designator);
    print_node(stream, _nodes[0]);
    print_node(stream, _nodes[1]);
}

void Inductor::print(std::ostream &stream) override {

}

void Resistor::print(std::ostream &stream) override {}

void Diode::print(std::ostream &stream) override {}

void Transistor::print(std::ostream &stream) override {}

void CurrentSource::print(std::ostream &stream) override {}

void VoltageSource::print(std::ostream &stream) override {}

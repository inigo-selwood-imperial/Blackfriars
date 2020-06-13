#pragma once

#include <memory>

#include "..\..\text_buffer.hpp"
#include "..\..\parse.hpp"

class Schematic;
class Operation;

class Component {

public:

    enum Type {
        NONE = -1,

        CAPACITOR,
        INDUCTOR,
        RESISTOR,

        CURRENT_SOURCE,
        VOLTAGE_SOURCE
    };

    std::string name;

    std::vector<std::string> nodes;

    Type type;

    Component() {
        nodes.resize(2);
        type = NONE;
    }

    void simulate(const std::shared_ptr<Operation> &operation, const Schematic &schematic,
            const double &time);

};

void Component::simulate(const std::shared_ptr<Operation> &operation, const Schematic &schematic,
        const double &time) {}

#pragma once

#include <memory>

#include "../../text_buffer.hpp"
#include "../../parse.hpp"

class Schematic;
class Operation;

class Component {

public:

    enum Type {
        NONE = -1,

        CAPACITOR = 1,
        INDUCTOR = 1 << 1,
        RESISTOR = 1 << 2,

        CURRENT_SOURCE = 1 << 3,
        VOLTAGE_SOURCE = 1 << 4
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

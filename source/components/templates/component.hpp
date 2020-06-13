#pragma once

#include <memory>

#include "../../text_buffer.hpp"
#include "../../parse.hpp"

class Schematic;
class Operation;

class Transient;

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

    virtual void simulate(const std::shared_ptr<Transient> &operation,
            const Schematic &schematic, const double &time) = 0;

};

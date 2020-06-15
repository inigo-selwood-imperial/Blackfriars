#pragma once

#include <memory>

#include "../../text_buffer.hpp"
#include "../../parse.hpp"
#include "../../hash.hpp"

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

    Hash hash;

    std::vector<std::string> node_names;

    std::vector<Hash> node_hashes;

    Type type;

    Component() {
        node_hashes.resize(2);
        node_names.resize(2);
        hash = 0;
        type = NONE;
    }

    virtual void simulate(const std::shared_ptr<Transient> &operation,
            const Schematic &schematic, const double &time) = 0;

};

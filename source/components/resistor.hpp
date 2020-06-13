#pragma once

#include "templates/passive.hpp"
#include "templates/component.hpp"

class Resistor : public Passive, public Component {

public:

    double value;

    static std::shared_ptr<Resistor> parse(TextBuffer &buffer);

    void simulate(const std::shared_ptr<Transient> &transient,
            const Schematic &schematic, const double &time) override;

};

std::shared_ptr<Resistor> Resistor::parse(TextBuffer &buffer) {
    return Passive::parse<Resistor>(buffer, 'R');
}

void Resistor::simulate(const std::shared_ptr<Transient> &transient,
        const Schematic &schematic, const double &time) {

    transient->add_resistance(nodes[0], nodes[1], value);
}

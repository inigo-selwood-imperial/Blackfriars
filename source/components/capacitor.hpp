#pragma once

#include "templates/component.hpp"
#include "templates/passive.hpp"

class Capacitor : public Passive, public Component {

public:

    double value;

    static std::shared_ptr<Capacitor> parse(TextBuffer &buffer);

    void simulate(const std::shared_ptr<Transient> &transient,
            const Schematic &schematic, const double &time) override;

};

std::shared_ptr<Capacitor> Capacitor::parse(TextBuffer &buffer) {
    return Passive::parse<Capacitor>(buffer, 'C');
}

void Capacitor::simulate(const std::shared_ptr<Transient> &transient,
        const Schematic &schematic, const double &time) {

    const double value = (1 / value) * transient->current_integral(this->name);
    transient->add_voltage(nodes[0], nodes[1], this->name, value);
}

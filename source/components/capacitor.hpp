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

    const double voltage = (1 / value) * transient->get_current_integral(
            hash);
    transient->add_voltage(node_hashes[0], node_hashes[1], hash, voltage);
}

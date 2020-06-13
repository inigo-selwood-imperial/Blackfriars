#pragma once

#include "templates/component.hpp"
#include "templates/passive.hpp"

class Capacitor : public Passive, public Component {

public:

    double value;

    static std::shared_ptr<Capacitor> parse(TextBuffer &buffer);

    void simulate(const std::shared_ptr<Transient> &transient,
            const Schematic &schematic, const double &time);

};

std::shared_ptr<Capacitor> Capacitor::parse(TextBuffer &buffer) {
    return Passive::parse<Capacitor>(buffer, 'C');
}

void Capacitor::simulate(const std::shared_ptr<Transient> &transient,
        const Schematic &schematic, const double &time) {}

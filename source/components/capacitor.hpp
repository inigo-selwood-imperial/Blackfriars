#pragma once

class Capacitor : public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Capacitor();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic,
            const double &time);

};

Capacitor::Capacitor() : Component(Component::Type::CAPACITOR) {}

bool Capacitor::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Passive::parse<Capacitor>(buffer, schematic, 'C');
}

template <Transient transient>
void Capacitor::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {
}

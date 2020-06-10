#pragma once

class Resistor : public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Resistor();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic,
            const double &time);

};

bool Resistor::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Passive::parse<Resistor>(buffer, schematic, 'R');
}

Resistor::Resistor() : Component(Component::Type::RESISTOR) {}

template <Transient transient>
void Resistor::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {
}

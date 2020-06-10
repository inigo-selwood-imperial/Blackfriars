#pragma once

class Inductor : public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Inductor();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic, const double &time);

};

bool Inductor::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Passive::parse<Inductor>(buffer, schematic, 'L');
}

Inductor::Inductor() : Component(Component::Type::INDUCTOR) {}

template <Transient transient>
void Inductor::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {
}

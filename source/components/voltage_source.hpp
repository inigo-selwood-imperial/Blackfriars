#pragma once

class VoltageSource : public Source, public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    VoltageSource();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic,
            const double &time);

};

bool VoltageSource::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Source::parse<VoltageSource>(buffer, schematic, 'V');
}

VoltageSource::VoltageSource() : Component(Component::Type::VOLTAGE_SOURCE) {}

template <Transient transient>
void VoltageSource::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {
}

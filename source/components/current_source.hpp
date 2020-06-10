#pragma once

class CurrentSource : public Source, public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    CurrentSource();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic,
            const double &time);

};

bool CurrentSource::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Source::parse<CurrentSource>(buffer, schematic, 'I');
}

CurrentSource::CurrentSource() : Component(Component::Type::CURRENT_SOURCE) {}

template <Transient transient>
void CurrentSource::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {
}

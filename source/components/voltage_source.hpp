#pragma once

#include "templates/component.hpp"
#include "templates/source.hpp"

class VoltageSource : public Source, public Component {

public:

    static std::shared_ptr<VoltageSource> parse(TextBuffer &buffer);

    void simulate(const std::shared_ptr<Transient> &transient,
            const Schematic &schematic, const double &time) override;

};

std::shared_ptr<VoltageSource> VoltageSource::parse(TextBuffer &buffer) {
    return Source::parse<VoltageSource>(buffer, 'V');
}

void VoltageSource::simulate(const std::shared_ptr<Transient> &transient,
        const Schematic &schematic, const double &time) {

    transient->add_voltage(node_hashes[0], node_hashes[1], hash, value(time));
}

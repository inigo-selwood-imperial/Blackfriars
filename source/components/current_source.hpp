#pragma once

#include "templates/component.hpp"
#include "templates/source.hpp"

class CurrentSource : public Source, public Component {

public:

    static std::shared_ptr<CurrentSource> parse(TextBuffer &buffer);

    void simulate(const std::shared_ptr<Transient> &transient,
            const Schematic &schematic, const double &time) override;

};

std::shared_ptr<CurrentSource> CurrentSource::parse(TextBuffer &buffer) {
    return Source::parse<CurrentSource>(buffer, 'I');
}

void CurrentSource::simulate(const std::shared_ptr<Transient> &transient,
        const Schematic &schematic, const double &time) {

    transient->add_current(nodes[0], nodes[1], name, value(time));
}

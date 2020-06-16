#pragma once

#include "templates/component.hpp"

class Diode : public Component {

public:

    static std::shared_ptr<Diode> parse(TextBuffer &buffer);

    void simulate(const std::shared_ptr<Transient> &transient,
            const Schematic &schematic, const double &time) override;

};

std::shared_ptr<Diode> Diode::parse(TextBuffer &buffer) {
    auto diode = std::shared_ptr<Diode>(new Diode());

    // Check the right parse function's been called
    if(buffer.get_character() != 'D') {
    std::cerr << "Parse logic error; expected a diode definition, but "
            "encountered the component symbol '" <<
            buffer.get_character() << "'" << std::endl;
    return nullptr;
    }

    // Extract the diode's name
    diode->name = buffer.get_string(true);
    diode->hash = hash_value(diode->name);

    // Extract the diode's nodes
    buffer.skip_whitespace();
    diode->node_names[0] = buffer.get_string(true);
    diode->node_hashes[0] = hash_node(diode->node_names[0]);
    buffer.skip_whitespace();
    diode->node_names[1] = buffer.get_string(true);
    diode->node_hashes[1] = hash_node(diode->node_names[1]);

    // Parse its value
    buffer.skip_whitespace();
    if(buffer.skip_character('D') == false) {
        std::cerr << "Expected a model name at the end of diode definition" <<
                std::endl;
        return nullptr;
    }

    return diode;
}

void Diode::simulate(const std::shared_ptr<Transient> &transient,
        const Schematic &schematic, const double &time) {

}

#pragma once

#include "component.hpp"

class Passive {

public:

    double value;

    template <typename PassiveType>
    static std::shared_ptr<PassiveType> parse(TextBuffer &buffer,
            const char &symbol);

};

// Parse a passive component of indeterminate type
template <typename PassiveType>
std::shared_ptr<PassiveType> Passive::parse(TextBuffer &buffer,
        const char &symbol) {

    auto passive = std::shared_ptr<PassiveType>(new PassiveType());

    static std::map<char, std::string> symbol_names = {
        {'C', "capacitor"},
        {'L', "inductor"},
        {'R', "resistor"}
    };

    // Check the symbol provided is for a resistor, capacitor, or inductor
    if(symbol_names.find(symbol) == symbol_names.end()) {
        std::cerr << "Parse logic error; called passive parse function for "
                "component with symbol '" << symbol << "', which isn't "
                "supported" << std::endl;
        return nullptr;
    }

    // Check the right parse function's been called
    if(buffer.get_character() != symbol) {
        std::cerr << "Parse logic error; expected a " << symbol_names[symbol] <<
                " definition, but encountered the component symbol '" <<
                buffer.get_character() << "'" << std::endl;
        return nullptr;
    }

    // Extract the passive's name
    passive->name = buffer.get_string(true);

    // Extract the passive's nodes
    buffer.skip_whitespace();
    passive->nodes[0] = buffer.get_string(true);
    buffer.skip_whitespace();
    passive->nodes[1] = buffer.get_string(true);

    // Parse its value
    buffer.skip_whitespace();
    const auto value_string = buffer.get_string(true);
    try {
        passive->value = parse_metric_value(value_string);
    }
    catch(...) {
        std::cerr << "Couldn't parse " << symbol_names[symbol] <<
                "'s value field" << std::endl;
        return nullptr;
    }

    return passive;
}

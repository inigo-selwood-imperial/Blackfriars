#pragma once

#include <memory>
#include <functional>
#include <string>

#include <cmath>

#include "component.hpp"

#include "../../utilities/hash.hpp"

class Function {

public:

    virtual double value(const double &time) const = 0;

    static std::shared_ptr<Function> parse(TextBuffer &buffer);

};

class Constant : public Function {

public:

    double offset;

    static std::shared_ptr<Constant> parse(TextBuffer &buffer);

    double value(const double &time) const override;

};

// Parse a constant value
std::shared_ptr<Constant> Constant::parse(TextBuffer &buffer) {
    auto constant = std::shared_ptr<Constant>(new Constant());

    const auto value = buffer.get_string(true);
    try {
        constant->offset = parse_metric_value(value);
    }
    catch(...) {
        std::cerr << "Couldn't parse constant value '" << value << "'" <<
                std::endl;
        return nullptr;
    }

    return constant;
}

double Constant::value(const double &time) const {
    return offset;
}

class Sinusoid : public Function {

public:

    double offset;
    double amplitude;
    double frequency;
    double delay;
    double theta; // Damping factor
    double phi; // Phase
    double cycles;

    static std::shared_ptr<Sinusoid> parse(TextBuffer &buffer);

    Sinusoid() {
        offset = 0;
        amplitude = 0;
        frequency = 0;
        delay = 0;
        theta = 1;
        phi = 0;
        cycles = 0;
    }

    double value(const double &time) const override;

};

// Parse a sinusoid function
std::shared_ptr<Sinusoid> Sinusoid::parse(TextBuffer &buffer) {
    auto sinusoid = std::shared_ptr<Sinusoid>(new Sinusoid());

    buffer.skip_string("SINE(");

    std::vector<std::reference_wrapper<double>> parameters = {
        sinusoid->offset,
        sinusoid->amplitude,
        sinusoid->frequency,
        sinusoid->delay,
        sinusoid->theta,
        sinusoid->phi,
        sinusoid->cycles
    };
    for(double &parameter : parameters) {
        buffer.skip_whitespace();
        const auto value = buffer.get_string(true, {' ', '\t', '\n', ')'});
        if(value.empty())
            break;

        try {
            parameter = parse_metric_value(value);
        }
        catch(...) {
            std::cerr << "Couldn't parse sine function parameter '" <<
                    std::endl;
            return nullptr;
        }
    }

    buffer.skip_whitespace();
    if(buffer.skip_character(')') == false) {
        std::cerr << "Syntax error in sine function" << std::endl;
        return nullptr;
    }

    return sinusoid;
}

// Return the value of the function at a given time
double Sinusoid::value(const double &time) const {
    if(time == -1)
        return offset;

    // Return if the delay time hasn't been reached
    if(time < delay)
        return 0;

    // Return if a cycle count has been set, and the cycle limit has been
    // reached
    else if(cycles && time > ((1 / frequency) * cycles + delay))
        return 0;

    // Do all kinds of complicated maths sh*te
    const double omega = 2 * 3.14159265359 * frequency;
    const double damping_factor = std::exp(-theta * (time - delay));
    const double sine = std::sin(omega * (time - delay) + phi);
    return amplitude * damping_factor * sine + offset;
}

// Parse a value of indeterminate type
std::shared_ptr<Function> Function::parse(TextBuffer &buffer) {
    if(buffer.get_string(false, {'('}) == "SINE")
        return Sinusoid::parse(buffer);
    else
        return Constant::parse(buffer);
}

// *********************************************************** Source base class

class Source {

public:

    std::shared_ptr<Function> function;

    template <typename SourceType>
    static std::shared_ptr<SourceType> parse(TextBuffer &buffer,
            const char &symbol);

    double value(const double &time) const;

};

// Parses a source definition
template <typename SourceType>
std::shared_ptr<SourceType> Source::parse(TextBuffer &buffer,
        const char &symbol) {

    auto source = std::shared_ptr<SourceType>(new SourceType());

    static std::map<char, std::string> symbol_names = {
        {'I', "current source"},
        {'V', "voltage source"}
    };

    // Check the symbol provided is for a resistor, capacitor, or inductor
    if(symbol_names.find(symbol) == symbol_names.end()) {
        std::cerr << "Parse logic error; called source parse function for "
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

    // Extract the source's name and nodes
    source->name = buffer.get_string(true);
    source->hash = hash_value(source->name);

    buffer.skip_whitespace();
    source->node_names[0] = buffer.get_string(true);
    source->node_hashes[0] = hash_node(source->node_names[0]);
    buffer.skip_whitespace();
    source->node_names[1] = buffer.get_string(true);
    source->node_hashes[1] = hash_node(source->node_names[1]);

    // Parse its function
    buffer.skip_whitespace();
    const auto function = Function::parse(buffer);
    if(function == nullptr) {
       std::cerr << "Couldn't parse " << symbol_names[symbol] <<
               "'s value field" << std::endl;
       return nullptr;
    }
    else
        source->function = function;

    // Add the source to the schmatic
    return source;
}

double Source::value(const double &time) const {

    // Check the source has a function specified
    if(function == nullptr)
        return 0;
    else
        return function->value(time);
}

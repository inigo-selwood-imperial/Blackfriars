#pragma once

class Component {

public:

    std::vector<std::string> nodes;

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

};

// ********************************************************** Passive base class

class Passive {

public:

    double value;

    template <typename Type>
    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Passive();

};

Passive::Passive() {
    value = 0;
}

// ************************************************************** Function class

class Function {

public:

    virtual double value(const double &time) const = 0;

    static std::shared_ptr<Function> parse(Parse::Buffer &buffer);

};

class Constant : public Function {

public:

    double offset;

    static std::shared_ptr<Constant> parse(Parse::Buffer &buffer);

    double value(const double &time) const override;

};

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

    double value(const double &time) const override;

};

double Sinusoid::value(const double &time) const {

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
    const double sine_value = std::sin(omega * (time - delay) + phi);
    return amplitude * damping_factor * sine_value + offset;
}

// *********************************************************** Source base class

class Source {

public:

    std::shared_ptr<Function> function;

    template <typename Type>
    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    double value(const double &time) const;

};

double Source::value(const double &time) const {

    // Check the source has a function specified
    if(function == nullptr)
        return 0;
    else
        return function->value(time);
}

// ************************************************************* Parse functions

template <typename Type>
bool Passive::parse(Parse::Buffer &buffer, Schematic &schematic) {
    auto passive = std::shared_ptr<Type>(new Type);

    // Extract the passive's name and nodes
    passive->name = buffer.skip_token();
    passive->nodes[0] = buffer.skip_token();
    passive->nodes[1] = buffer.skip_token();

    // Parse the component's values
    try {
        passive->value = Parse::metric_value(buffer.skip_token());
    }
    catch(const Parse::exception &exception) {
        return false;
    }

    // Check the end of the line's been reached
    if(buffer.skip_token('\n') == false)
        return false;

    // Add the component to the schematic
    schematic.add_component(passive);
    return true;
}

std::shared_ptr<Function> Function::parse(Parse::Buffer &buffer) {
    if(buffer.get_token("SINE")
        return Sinusoid::parse(buffer);
    else
        return Constant::parse(buffer);
}

std::shared_ptr<Constant> Constant::parse(Parse::Buffer &buffer) {
    auto constant = std::shared_ptr<Constant>(new Constant());

    const auto value = buffer.get_token();
    try {
        constant->value = Parse::metric_value(value);
    }
    catch(...)
        return nullptr;

    if(buffer.get_token('\n') == false)
        return nullptr;

    return constant;
}

std::shared_ptr<Sinusoid> Sinusoid::parse(Parse::Buffer &buffer) {
    sinusoid = std::shared_ptr<Sinusoid>(new Sinusoid());

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
        const auto value = buffer.skip_token({' ', '\t', ')'});
        if(value.empty())
            break;

        try {
            parameter = Parse::metric_value(parameter);
        }
        catch(...) {
            return nullptr;
        }
    }

    if(buffer.skip_token(')') == false)
        return nullptr;

    return sinusoid;
}

template <typename Type>
bool Source::parse(Parse::Buffer &buffer, Schematic &schematic) {
    auto source = std::shared_ptr<Type>(new Type);

    // Extract the source's name and nodes
    source->name = buffer.skip_token();
    source->nodes[0] = buffer.skip_token();
    source->nodes[1] = buffer.skip_token();

    // Parse its function
    const auto function = Function::parse(buffer);
    if(function == nullptr)
        return false;
    else
        source->function = function;

    // Check the end of the line's been reached
    if(buffer.skip_token('\n') == false)
        return false;

    // Add the source to the schmatic
    schematic.add_component(source);
    return true;
}

bool Component::parse(Parse::Buffer &buffer, Schematic &schematic) {
    if(buffer.get_token('C'))
        return Capacitor::parse(buffer, schematic);
    else if(buffer.get_token('L'))
        return Inductor::parse(buffer, schematic);
    else if(buffer.get_token('R'))
        return Resistor::parse(buffer, schematic);
    else if(buffer.get_token('V'))
        return VoltageSource::parse(buffer, schematic);
    else if(buffer.get_token('I'))
        return CurrentSource::parse(buffer, schematic);
    else
        return false;
}

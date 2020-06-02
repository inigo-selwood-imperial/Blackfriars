#pragma once

#include <functional>
#include <iostream> // std::endl
#include <locale> // std::tolower
#include <memory> // std::shared_ptr

#include <cmath>

#include "parse.hpp"

/*
This header file contains the definitions for all the types of components
expected to be encountered, including:
    (1) passives
        a) capacitors
        b) inductors
        c) resistors
    (2) semiconductors
        a) diodes
        b) transistors
    (3) sources
        a) current sources
        b) voltage sources

The base class has a few of the common features of all components, such as
a designator, and a vector of nodes (which will have two parts, other than in
the case of a transistor).

The derived components implement static parse functions, which in combination
with a parse buffer containing a SPICE netlist, can be used to create
components in their intermediate forms

Each component has a Type member field, which can be used to perform basic
typecast safety checks
*/

// ******************************************************** Component base class

class Component {

public:

    enum Type {
        CAPACITOR,
        INDUCTOR,
        RESISTOR,

        DIODE,
        TRANSISTOR,

        CURRENT_SOURCE,
        VOLTAGE_SOURCE
    };

    typedef std::shared_ptr<Component> Pointer;

    const Component::Type type;

    unsigned int designator;

    std::vector<unsigned int> nodes;

    static Pointer parse(Parse::Buffer &buffer);

    Component(const Type &type) : type(type) {
        designator = 0;
        nodes.resize(2);
    }

};

template <typename DerivedType, Component::Type type_>
class ComponentProxy : public Component {

public:

    typedef std::shared_ptr<DerivedType> Pointer;

    ComponentProxy() : Component(type_) {}

};

// ******************************************************************** Passives

class Passive {

public:

    double value;

    template <typename PassiveType>
    static typename PassiveType::Pointer parse(Parse::Buffer &buffer,
            const char &designator_prefix);

};

class Capacitor : public Passive,
        public ComponentProxy<Capacitor, Component::CAPACITOR> {

public:

    double charge;

    static Pointer parse(Parse::Buffer &buffer);

};

class Inductor : public Passive,
        public ComponentProxy<Inductor, Component::INDUCTOR> {

public:

    static Pointer parse(Parse::Buffer &buffer);

};

class Resistor : public Passive,
        public ComponentProxy<Resistor, Component::RESISTOR> {

public:

    static Pointer parse(Parse::Buffer &buffer);

};

// ************************************************************** Semiconductors

class Semiconductor {};

class Diode : public Semiconductor,
        public ComponentProxy<Diode, Component::DIODE> {

public:

    static Pointer parse(Parse::Buffer &buffer);

};

class Transistor : public Semiconductor,
        public ComponentProxy<Transistor, Component::TRANSISTOR> {

public:

    enum Model {
        NPN,
        PNP
    };

    Model model;

    Transistor() {
        nodes.resize(3);
    }

    static Pointer parse(Parse::Buffer &buffer);

};

// ********************************************************************* Sources

class Source {

public:

    double amplitude;
    double cycles;
    double damping_factor;
    double delay;
    double frequency;
    double offset;
    double phase;

    template <typename SourceType>
    static typename SourceType::Pointer parse(Parse::Buffer &buffer,
            const char &designator_prefix);

    Source();

};

class CurrentSource : public Source,
        public ComponentProxy<CurrentSource, Component::CURRENT_SOURCE> {

public:

    static Pointer parse(Parse::Buffer &buffer);

};

class VoltageSource : public Source,
        public ComponentProxy<VoltageSource, Component::VOLTAGE_SOURCE> {

public:

    static Pointer parse(Parse::Buffer &buffer);

};

/*
From this point on, come the function definitions for each of the classes
defined above. They're split up into 7 categories:

    (1) Constructors

    (2) Character identifiers
        These functions are used by the unspecialized to identify things like
        integers

    (3) Unspecialized parsers
        Unlike the functions parse_node and parse_value, these functions are
        used to perform more general functions like parse_number, which can
        be specialized for different tasks

    (4) Specialized parsers
        Helpers which handle parsing common fields, such as node number, and
        metric format values

    (5) Parse templates
        These templates are abstract functions of some of the components' base
        classes, which implement the general form of their derived classes'
        parse functions

    (6) Parse functions
        The static parse functions of the individual derived component classes,
        most often an implementation of a parse template (except in the cases
        of semiconductors like diodes and transistors)

    (7) Parse delegator
        This function is the most generic case of a parse function, where the
        program expects to find a component definition within the parse buffer,
        but does not care what type of component it is. It will return an
        unspecialized pointer which can be downcast later
*/

// **************************************************************** Constructors

Source::Source() {
    amplitude = 0;
    cycles = 0;
    damping_factor = 0;
    delay = 0;
    frequency = 0;
    offset = 0;
    phase = 0;
}

// ******************************************************* Character identifiers

// Returns true if the character is 0-9
static bool is_integer(const char &character) {
    return character >= '0' && character <= '9';
}

// Returns true if the character is 0-9 or a decimal point
static bool is_number(const char &character) {
    return (character >= '0' && character <= '9') || character == '.';
}

// Returns true if the character is part of a metric suffix (pico, nano, etc.)
static bool inline is_suffix(const char &character) {
    auto case_character = std::tolower(character);
    return case_character == 'f' || case_character == 'p' ||
            case_character == 'n' || case_character == 'u' ||
            case_character == 'm' || case_character == 'k' ||
            case_character == 'g' || case_character == 't';
}

// ******************************************************* Unspecialized parsers

// Returns the power of 10 represented by a metric suffix
static int inline parse_suffix(Parse::Buffer &buffer) {
    if(buffer.skip_string("Meg") == "Meg")
        return 6;

    switch(std::tolower(buffer.skip_current())) {
        case 'f':
            return -15;
        case 'p':
            return -12;
        case 'n':
            return -9;
        case 'u':
            return -6;
        case 'm':
            return -3;
        case 'k':
            return 3;
        case 'g':
            return 9;
        case 't':
            return 12;
    }

    std::cerr << "Failed to parse metrix suffix";
    throw -1;
}

// Parses an integer value, ignoring suffixed zeros
static unsigned int parse_integer(Parse::Buffer &buffer) {
    std::string value;
    while(is_integer(buffer.get_current()))
        value += buffer.skip_current();

    try {
        return std::stoi(value);
    }
    catch(...) {
        std::cerr << "Failed to convert string to floating point numner" <<
                std::endl;
        throw -1;
    }
}

// Parses a number (not including metric suffixes)
static double parse_number(Parse::Buffer &buffer) {
    std::string value;
    while(is_number(buffer.get_current()))
        value += buffer.skip_current();

    try {
        return std::stof(value);
    }
    catch(...) {
        std::cerr << "Failed to convert string to integer" << std::endl;
        throw -1;
    }

    // Prevents the compiler from complaining
    return 0;
}

// ********************************************************* Specialized parsers

// Parses a node, ignoring the suffixed 'N', and leading zeros
static unsigned int parse_node(Parse::Buffer &buffer) {
    if(buffer.skip_character('N') == false) {
        std::cerr << "Expected to find node definition in text";
        throw -1;
    }
    return parse_number(buffer);
}

/*
Parse a value, with decimal points and metric suffixes

A value is composed of up to three parts. A first number, an optional metrix
suffix, and a further optional second number -- all sandwiched together with no
spaces.

Some examples of the same number in different notation are: 1200, 1.2k, and 1k2
*/
static double parse_value(Parse::Buffer &buffer) {
    std::string string_value;

    while(is_number(buffer.get_current()))
        string_value += buffer.skip_current();

    auto factor = 0;
    if(is_suffix(buffer.get_current()))
        factor = parse_suffix(buffer);

    if(is_number(buffer.get_current()) && factor)
        string_value += ".";

    while(is_number(buffer.get_current()))
        string_value += buffer.skip_current();

    try {
        const double value = std::stof(string_value);
        return value * std::pow(10, factor);
    }
    catch(...) {
        std::cerr << "Couldn't parse metric-suffixed value" << std::endl;
        throw -1;
    }
}

// ************************************************************* Parse templates

// Parses a passive component of indeterminate type
template <typename PassiveType>
typename PassiveType::Pointer Passive::parse(Parse::Buffer &buffer,
        const char &designator_prefix) {

    // Check the right function has been called for the current buffer
    // character
    if(buffer.skip_character(designator_prefix) == false) {
        std::cerr << "Expected a component definition within buffer" <<
                std::endl;
        throw -1;
    }

    // Create a new source
    auto source = typename PassiveType::Pointer(new PassiveType());
    source->designator = parse_integer(buffer);

    // Parse the nodes it's connected to
    buffer.skip_whitespace();
    source->nodes[0] = parse_node(buffer);
    buffer.skip_whitespace();
    source->nodes[1] = parse_node(buffer);

    // Parse its value
    buffer.skip_whitespace();
    source->value = parse_value(buffer);

    return source;
}

// Parses a source definition of indeterminate type
template <typename SourceType>
typename SourceType::Pointer Source::parse(Parse::Buffer &buffer,
        const char &designator_prefix) {

    // Check the right function has been called for the current buffer
    // character
    if(buffer.skip_character(designator_prefix) == false) {
        std::cerr << "Expected a component definition within buffer" <<
                std::endl;
        throw -1;
    }

    // Parse the source's designator (V1, I2, etc.)
    auto source = typename SourceType::Pointer(new SourceType());
    source->designator = parse_integer(buffer);

    // Parse the two nodes to which the source is connected (N001, etc.)
    buffer.skip_whitespace();
    source->nodes[0] = parse_node(buffer);
    buffer.skip_whitespace();
    source->nodes[1] = parse_node(buffer);

    // The source can have either a constant value, or a sinusoidal function
    // (for the purposes of this application). Delegate to the relevant
    // case
    // NOTE: For the purposes of a constant value, the source is treated as
    // a sinusoidal one with an amplitude and frequency of zero, and a constant
    // offset
    buffer.skip_whitespace();
    if(buffer.skip_string("SINE").empty() == false) {
        buffer.skip_character('(');

        // As much as I would like to alphabetize these references, the gods of
        // LTSpice format have dictated that they appear in this order
        std::vector<std::reference_wrapper<double>> fields = {
            source->offset,
            source->amplitude,
            source->frequency,
            source->delay,
            source->damping_factor,
            source->phase,
            source->cycles
        };

        // The number of fields in any one sine function varies. Only the ones
        // set will have values, those preceding it will be set to 0, and any
        // that might have come after will be omitted. Therefore, we want to
        // parse fields up until a non-value field is encountered (a ')'
        // character, if the text is formatted properly)
        for(double &field : fields) {
            buffer.skip_whitespace();
            const auto character = buffer.get_current();
            if((character >= '0' && character <= '9') || character == '.')
                field = parse_value(buffer);
            else
                break;
        }

        // Skip the closing bracket
        if(buffer.skip_character(')') == false) {
            std::cerr << "Malformed power source sine wave definition" <<
                    std::endl;
            throw -1;
        }
    }

    // If the source is not sinusoidal, we want to set its constant value as
    // the source's DC offset.
    else
        source->offset = parse_value(buffer);

    return source;
}

// ************************************************************* Parse functions

// Parses a capacitor definition
Capacitor::Pointer Capacitor::parse(Parse::Buffer &buffer) {
    return Passive::parse<Capacitor>(buffer, 'C');
}

// Parses an inductor definition
Inductor::Pointer Inductor::parse(Parse::Buffer &buffer) {
    return Passive::parse<Inductor>(buffer, 'L');
}

// Parses a resistor definition
Resistor::Pointer Resistor::parse(Parse::Buffer &buffer) {
    return Passive::parse<Resistor>(buffer, 'R');
}

// Parses a transistor definition
Diode::Pointer Diode::parse(Parse::Buffer &buffer) {

    // Check the current buffer character is the start of a diode definition
    if(buffer.skip_character('D') == false) {
        std::cerr << "Expected diode definition in buffer" << std::endl;
        throw -1;
    }

    // Create a new diode component, and parse its designator
    auto diode = Diode::Pointer(new Diode());
    diode->designator = parse_integer(buffer);

    // Parse the nodes it's connected to
    buffer.skip_whitespace();
    diode->nodes[0] = parse_node(buffer);
    buffer.skip_whitespace();
    diode->nodes[1] = parse_node(buffer);

    // The final field will only ever be 'D' (for the purposes of this
    // application), but could be different if a non-standard diode was to be
    // simulated
    if(buffer.skip_character('D') == false) {
        std::cerr << "Expected a 'D' model specifier at the end of a diode"
                "definition" << std::endl;
        throw -1;
    }

    return diode;
}

// Parses a transistor definition
Transistor::Pointer Transistor::parse(Parse::Buffer &buffer) {

    // Check the current buffer character is the start of a transistor
    // definition
    if(buffer.skip_character('Q') == false) {
        std::cerr << "Expected transistor definition at current position in "
                "buffer" << std::endl;
        throw -1;
    }

    // Create a new transistor component and parse its designator
    auto transistor = Transistor::Pointer(new Transistor());
    transistor->designator = parse_integer(buffer);

    // Parse the nodes it's connected to (in the order: BCE)
    // TODO: Check that's actually the right order, I'm just guessing
    buffer.skip_whitespace();
    transistor->nodes[0] = parse_node(buffer);
    buffer.skip_whitespace();
    transistor->nodes[1] = parse_node(buffer);
    buffer.skip_whitespace();
    transistor->nodes[2] = parse_node(buffer);

    // Transistors can have two different models: NPN and PNP (for the
    // purposes of this application, in actual LTSpice netlists, there can
    // be a model name here, eg: 2N2222).
    if(buffer.skip_string("NPN").empty() == false)
        transistor->model = NPN;
    else if(buffer.skip_string("PNP").empty() == false)
        transistor->model = PNP;
    else {
        std::cerr << "Expected a 'NPN' or 'PNP' model specifier at the end of "
                "a diode definition" << std::endl;
        throw -1;
    }

    return transistor;

}

// Parses a current source definition
CurrentSource::Pointer CurrentSource::parse(Parse::Buffer &buffer) {
    return Source::parse<CurrentSource>(buffer, 'I');
}

// Parses a voltage source definition
VoltageSource::Pointer VoltageSource::parse(Parse::Buffer &buffer) {
    return Source::parse<VoltageSource>(buffer, 'V');
}

// ************************************************************* Parse delegator

// Parse a component of indeterminate type
Component::Pointer Component::parse(Parse::Buffer &buffer) {

    // All components begin with a designator (a letter indicating the type
    // of component, followed by an integer which distinguishes it from other
    // components of the same type). Delegate to the relevant parse function,
    // and return a pointer to the newly constructed component
    switch(buffer.get_current()) {
        case 'C':
            return Capacitor::parse(buffer);
        case 'D':
            return Diode::parse(buffer);
        case 'I':
            return CurrentSource::parse(buffer);
        case 'L':
            return Inductor::parse(buffer);
        case 'Q':
            return Transistor::parse(buffer);
        case 'R':
            return Resistor::parse(buffer);
        case 'V':
            return VoltageSource::parse(buffer);
        default:
            std::cerr << "Couldn't find a component which matches the "
                    "designator prefix" << std::endl;
            throw -1;
    };
}

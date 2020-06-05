#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include "log.hpp"
#include "parse.hpp"

/* ******************************************************************** Synopsis

Components are the heart of this application. Each electrical component that is
expected to appear in the SPICE netlists handled has its own class in this
header file.

In turn, each of the components has its own specialized parse and print
functions, which construct or serialize it from/to SPICE netlist format. The
classes also have some specific member fields, like an extra node for
transistors, or a function for voltage/current sources.

By deriving each component from a base component class, the Simulation class
(previously the deprecated Schematic class) can hold a polymorphic pointer to
it, and the enumerated Type field gets rid of the need for computationally
expensive type-casting or RTTI.

The source file is split up into 7 parts:

    (1) Component definition

    (2) Passive definitions
        Definitions for the Resistor, Capacitor, and Inductor classes, as well
        as a base Passive class

    (3) Source definitions
        Definitions for the VoltageSource and CurrentSource classes, as well as
        a base Source class

    (4) Semiconductor definitions
        Definitions for the Diode and Transistor classes

    (5) Parse templates
        A lot of the code for passive components or source components only
        varies by a single designator letter, so these templates allow the
        derived components to avoid duplicate code by calling a static function
        in their parent classes, which uses the same formatting, changing
        the designator prefix ('D' for diode, 'I' for current source, etc.)

    (6) Parse functions
        The definitions of the static parse functions in each component class

    (7) Constructors

*/

// ******************************************************** Component definition

class Component {

public:

    enum Type {
        CAPACITOR,
        INDUCTOR,
        RESISTOR,

        CURRENT_SOURCE,
        VOLTAGE_SOURCE,

        DIODE,
        TRANSISTOR
    };

    Type type;

    unsigned int designator;

    std::vector<unsigned int> nodes;

    static std::shared_ptr<Component> parse(Parse::Buffer &buffer);

    static unsigned int parse_node(Parse::Buffer &buffer);

    Component(const Type &type_);

};

template <typename DerivedType, Component::Type type_>
class ComponentProxy : public Component {

public:

    ComponentProxy() : Component(type_) {}

};

/* ********************************************************* Passive definitions

Passive components are the simplest types of components, with only a value field
(for resistors, the resistance in Ohms, for capacitors, the capacitance in
Farads, etc.)

*/

class Passive {

public:

    double value;

    template <typename PassiveType>
    static std::shared_ptr<PassiveType> parse(Parse::Buffer &buffer,
            const char &designator_prefix);

    Passive();

};

class Capacitor : public Passive,
        public ComponentProxy<Capacitor, Component::CAPACITOR> {

public:

    static std::shared_ptr<Capacitor> parse(Parse::Buffer &buffer);

};

class Inductor : public Passive,
        public ComponentProxy<Inductor, Component::INDUCTOR> {

public:

    static std::shared_ptr<Inductor> parse(Parse::Buffer &buffer);

};

class Resistor : public Passive,
        public ComponentProxy<Resistor, Component::RESISTOR> {

public:

    static std::shared_ptr<Resistor> parse(Parse::Buffer &buffer);

};

/* ********************************************************** Source definitions

Sources are distinct from passive components in that they can have varying
values as a function of time. For the purposes of this application, we are only
concerned with sine functions, the exact format of which is laid out in the
Source::Function class.

For sources whose values *don't* vary as a function of time, the same function
class is used, but all fields other than the DC offset are left zero'd

*/
class Source {

public:

    // Of the form: SINE(dc_offset amplitude frequency delay theta phi n_cycles)
    struct Function {

        double amplitude;
        double cycle_count;
        double delay;
        double frequency;
        double offset;
        double phi;
        double theta;

        static Function parse(Parse::Buffer &buffer);

        Function();

    };

    Function function;

    template <typename SourceType>
    static std::shared_ptr<SourceType> parse(Parse::Buffer &buffer,
            const char &designator_prefix);

};

class CurrentSource : public Source,
        public ComponentProxy<CurrentSource, Component::CURRENT_SOURCE> {

public:

    static std::shared_ptr<CurrentSource> parse(Parse::Buffer &buffer);

};

class VoltageSource : public Source,
        public ComponentProxy<VoltageSource, Component::VOLTAGE_SOURCE> {

public:

    static std::shared_ptr<VoltageSource> parse(Parse::Buffer &buffer);

};

// *************************************************** Semiconductor definitions

class Diode : public ComponentProxy<Diode, Component::DIODE> {

public:

    static std::shared_ptr<Diode> parse(Parse::Buffer &buffer);

};

class Transistor : public ComponentProxy<Transistor, Component::TRANSISTOR> {

public:

    enum Model {
        NONE,

        NPN,
        PNP
    };

    Model model;

    static std::shared_ptr<Transistor> parse(Parse::Buffer &buffer);

    Transistor();

};

// ************************************************************* Parse templates

// Parses a node definition in the form 'Nxxx' where xxx is a natural number
unsigned int Component::parse_node(Parse::Buffer &buffer) {
    if(buffer.skip_character('0'))
        return 0;
    else if(buffer.skip_character('N') == false) {
        Log::error() << "Expected a node definition " <<
                buffer.get_position() << std::endl;
        throw -1;
    }

    return Parse::natural_number(buffer);
}

// Parses a function, either constant or sinusoidal
Source::Function Source::Function::parse(Parse::Buffer &buffer) {

    Function function;

    // The source can have either a constant value, or a sinusoidal function
    // (for the purposes of this application). Delegate to the relevant
    // case
    // NOTE: For the purposes of a constant value, the source is treated as
    // a sinusoidal one with an amplitude and frequency of zero, and a constant
    // offset
    buffer.skip_whitespace();
    if(buffer.skip_string("SINE")) {
        buffer.skip_character('(');

        // As much as I would like to alphabetize these references, the gods of
        // LTSpice format have dictated that they appear in this order
        std::vector<std::reference_wrapper<double>> fields = {
            function.offset,
            function.amplitude,
            function.frequency,
            function.delay,
            function.theta,
            function.phi,
            function.cycle_count
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
                field = Parse::metric_value(buffer);
            else
                break;
        }

        // Skip the closing bracket
        if(buffer.skip_character(')') == false) {
            Log::error() << "Malformed source sine function " <<
                    buffer.get_position() << std::endl;
            throw -1;
        }
    }

    // If the source is not sinusoidal, we want to set its constant value as
    // the source's DC offset.
    else
        function.offset = Parse::metric_value(buffer);

    return function;
}

// Parses a passive component of indeterminate type
template <typename PassiveType>
std::shared_ptr<PassiveType> Passive::parse(Parse::Buffer &buffer,
        const char &designator_prefix) {

    if(buffer.skip_character(designator_prefix) == false) {
        Log::error() << "Expected a passive definition " <<
                buffer.get_position() << std::endl;
        throw -1;
    }

    // Create a new passive
    auto passive = typename std::shared_ptr<PassiveType>(new PassiveType());
    passive->designator = Parse::natural_number(buffer);

    // Parse the nodes it's connected to
    buffer.skip_whitespace();
    passive->nodes[0] = Component::parse_node(buffer);
    buffer.skip_whitespace();
    passive->nodes[1] = Component::parse_node(buffer);

    // Parse its value
    buffer.skip_whitespace();
    passive->value = Parse::metric_value(buffer);

    return passive;
}

// Parses a source of indeterminate type
template <typename SourceType>
std::shared_ptr<SourceType> Source::parse(Parse::Buffer &buffer,
        const char &designator_prefix) {

    // Check the right function has been called for the current buffer
    // character
    if(buffer.skip_character(designator_prefix) == false) {
        Log::error() << "Expected a source definition " <<
                buffer.get_position() << std::endl;
        throw -1;
    }

    // Parse the source's designator (V1, I2, etc.)
    auto source = typename std::shared_ptr<SourceType>(new SourceType());
    source->designator = Parse::natural_number(buffer);

    // Parse the two nodes to which the source is connected (N001, etc.)
    buffer.skip_whitespace();
    source->nodes[0] = Component::parse_node(buffer);
    buffer.skip_whitespace();
    source->nodes[1] = Component::parse_node(buffer);

    // Parse the source's function (constant, sinusoidal, or any number of
    // other fancy LTSpice options which we aren't considering in this
    // application)
    source->function = Function::parse(buffer);

    return source;
}

// ************************************************************* Parse functions

// Parses a component of indeterminate type
std::shared_ptr<Component> Component::parse(Parse::Buffer &buffer) {
    switch(buffer.get_current()) {
        case 'C':
            return Capacitor::parse(buffer);

        case 'L':
            return Inductor::parse(buffer);

        case 'R':
            return Resistor::parse(buffer);

        case 'V':
            return VoltageSource::parse(buffer);

        case 'I':
            return CurrentSource::parse(buffer);

        case 'D':
            return Diode::parse(buffer);

        case 'Q':
            return Transistor::parse(buffer);
    }

    Log::error() << "Expected a component definition " <<
            buffer.get_position() << std::endl;
    throw -1;
}

// Parses a capacitor definition
std::shared_ptr<Capacitor> Capacitor::parse(Parse::Buffer &buffer) {
    return Passive::parse<Capacitor>(buffer, 'C');
}

// Parses an inductor definition
std::shared_ptr<Inductor> Inductor::parse(Parse::Buffer &buffer) {
    return Passive::parse<Inductor>(buffer, 'L');
}

// Parses a  resistor definition
std::shared_ptr<Resistor> Resistor::parse(Parse::Buffer &buffer) {
    return Passive::parse<Resistor>(buffer, 'R');
}

// Parses a voltage source definition
std::shared_ptr<VoltageSource> VoltageSource::parse(Parse::Buffer &buffer) {
    return Source::parse<VoltageSource>(buffer, 'V');
}

// Parses a current source definition
std::shared_ptr<CurrentSource> CurrentSource::parse(Parse::Buffer &buffer) {
    return Source::parse<CurrentSource>(buffer, 'I');
}

// Parses a diode definition
std::shared_ptr<Diode> Diode::parse(Parse::Buffer &buffer) {

    // Check the current buffer character is the start of a diode definition
    if(buffer.skip_character('D') == false) {
        Log::error() << "Expected a diode definition " <<
                buffer.get_position() << std::endl;
        throw -1;
    }

    // Create a new diode component, and parse its designator
    auto diode = std::shared_ptr<Diode>(new Diode());
    diode->designator = Parse::natural_number(buffer);

    // Parse the nodes it's connected to
    buffer.skip_whitespace();
    diode->nodes[0] = Component::parse_node(buffer);
    buffer.skip_whitespace();
    diode->nodes[1] = Component::parse_node(buffer);

    // The final field will only ever be 'D' (for the purposes of this
    // application), but could be different if a non-standard diode was to be
    // simulated
    if(buffer.skip_character('D') == false) {
        Log::error() << "Diode definitions should end with a model name 'D' " <<
                buffer.get_position() << std::endl;
        throw -1;
    }

    return diode;
}

// Parses a transistor definition
std::shared_ptr<Transistor> Transistor::parse(Parse::Buffer &buffer) {

    // Check the current buffer character is the start of a transistor
    // definition
    if(buffer.skip_character('Q') == false) {
        Log::error() << "Expected a transistor definition " <<
                buffer.get_position() << std::endl;
        throw -1;
    }

    // Create a new transistor component and parse its designator
    auto transistor = std::shared_ptr<Transistor>(new Transistor());
    transistor->designator = Parse::natural_number(buffer);

    // Parse the nodes it's connected to (in the order: BCE)
    // TODO: Check that's actually the right order, I'm just guessing
    buffer.skip_whitespace();
    transistor->nodes[0] = Component::parse_node(buffer);
    buffer.skip_whitespace();
    transistor->nodes[1] = Component::parse_node(buffer);
    buffer.skip_whitespace();
    transistor->nodes[2] = Component::parse_node(buffer);

    // Transistors can have two different models: NPN and PNP (for the
    // purposes of this application, in actual LTSpice netlists, there can
    // be a model name here, eg: 2N2222).
    if(buffer.skip_string("NPN"))
        transistor->model = NPN;
    else if(buffer.skip_string("PNP"))
        transistor->model = PNP;
    else {
        Log::error() << "Transistor definitions should end with a model "
                "name " << buffer.get_position() << std::endl;
        throw -1;
    }

    return transistor;
}

// **************************************************************** Constructors

Component::Component(const Type &type_) : type(type_) {
    designator = 0;
    nodes.resize(2, 0);
}

Passive::Passive() {
    value = 0;
}

Source::Function::Function() {
    amplitude = 0;
    cycle_count = 0;
    delay = 0;
    frequency = 0;
    offset = 0;
    phi = 0;
    theta = 0;
}

Transistor::Transistor() {
    nodes.resize(3, 0);
    model = NONE;
}

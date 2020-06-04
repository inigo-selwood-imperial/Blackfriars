#pragma once

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

The source file is split up into 9 parts:

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

    (8) Print templates
        As with the parse templtes, these static functions in the base classes
        help prevent duplicate code in their derived classes

    (9) Print functions
        The definitions of the print functions in each component class

*/

// ******************************************************** Component definition

template <typename Type>
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

    static void print_node(std::ostream &stream);

    Component(const Type &type_);

};

/* ********************************************************* Passive definitions

Passive components are the simplest types of components, with only a value field
(for resistors, the resistance in Ohms, for capacitors, the capacitance in
Farads, etc.)

*/

class Passive {

public:

    double value;

    template <typename Type>
    static std::shared_ptr<Type> parse(Parse::Buffer &buffer);

    static void print(std::ostream &stream,
            const std::shared_ptr<Passive> &passive,
            const char &designator_prefix);

    Passive();

};

class Capacitor : public Passive, public Component<Capacitor> {

public:

    static Capacitor parse(Parse::Buffer &buffer);

    Capacitor();

    void print(std::ostream &stream);

};

class Inductor : public Passive, public Component<Inductor> {

public:

    static Capacitor parse(Parse::Buffer &buffer);

    Inductor();

    void print(std::ostream &stream);

};

class Resistor : public Passive, public Component<Resistor> {

public:

    static Capacitor parse(Parse::Buffer &buffer);

    Resistor();

    void print(std::ostream &stream);

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

    template <typename Type>
    static std::shared_ptr<Type> parse(Parse::Buffer &buffer);

    static void print(std::ostream &stream,
            const std::shared_ptr<Source> &source,
            const char &designator_prefix);

};

class CurrentSource : public Source, public Component<CurrentSource> {

public:

    static Capacitor parse(Parse::Buffer &buffer);

    CurrentSource();

    void print(std::ostream &stream);

};

class VoltageSource : public Source, public Component<VoltageSource> {

public:

    static Capacitor parse(Parse::Buffer &buffer);

    VoltageSource();

    void print(std::ostream &stream);

};

// *************************************************** Semiconductor definitions

class Diode : public Component<Diode> {

public:

    static Capacitor parse(Parse::Buffer &buffer);

    Diode();

    void print(std::ostream &stream);

};

class Transistor : public Component<Transistor> {

public:

    enum Model {
        NONE,

        NPN,
        PNP
    };

    Model model;

    static Capacitor parse(Parse::Buffer &buffer);

    Transistor();

    void print(std::ostream &stream);

};

// ************************************************************* Parse templates

unsigned int Component::parse_node(Parse::Buffer &buffer) {}

template <typename Type>
std::shared_ptr<Type> Passive::parse(Parse::Buffer &buffer) {}

template <typename Type>
std::shared_ptr<Type> Source::parse(Parse::Buffer &buffer) {}

// ************************************************************* Parse functions

std::shared_ptr<Component> Component::parse(Parse::Buffer &buffer) {}

std::shared_ptr<Capacitor> Capacitor::parse(Parse::Buffer &buffer) {}

std::shared_ptr<Inductor> Inductor::parse(Parse::Buffer &buffer) {}

std::shared_ptr<Resistor> Resistor::parse(Parse::Buffer &buffer) {}

std::shared_ptr<VoltageSource> VoltageSource::parse(Parse::Buffer &buffer) {}

std::shared_ptr<CurrentSource> CurrentSource::parse(Parse::Buffer &buffer) {}

std::shared_ptr<Diode> Diode::parse(Parse::Buffer &buffer) {}

std::shared_ptr<Transistor> Transistor::parse(Parse::Buffer &buffer) {}

// **************************************************************** Constructors

Component::Component(const Type &type_) : type(type_) {
    designator = 0;
    nodes.resize(2, 0);
}

Passive::Passive() {
    value = 0;
}

Capacitor::Capacitor() : Component(CAPACITOR) {}

Inductor::Inductor() : Inductor(INDUCTOR) {}

Resistor::Resistor() : Component(RESISTOR) {}

Source::Function::Function() {
    amplitude = 0;
    cycle_count = 0;
    delay = 0;
    frequency = 0;
    offset = 0;
    phi = 0;
    theta = 0;
}

CurrentSource::CurrentSource() : Component(CURRENT_SOURCE) {}

VoltageSource::VoltageSource() : Component(VOLTAGE_SOURCE) {}

Diode::Diode() : Component(DIODE) {}

Transistor::Transistor() : Component(TRANSISTOR) {
    nodes.resize(3, 0);
    model = NONE;
}

// ************************************************************* Print templates

void Component::print_node(std::ostream &stream) {}

void Passive::print(std::ostream &stream,
        const std::shared_ptr<Passive> &passive,
        const char &designator_prefix) {}

void Source::print(std::ostream &stream, const std::shared_ptr<Source> &source,
        const char &designator_prefix) {}

// ************************************************************* Print functions

void Capacitor::print(std::ostream &stream) {}

void Inductor::print(std::ostream &stream) {}

void Resistor::print(std::ostream &stream) {}

void VoltageSource::print(std::ostream &stream) {}

void CurrentSource::print(std::ostream &stream) {}

void Diode::print(std::ostream &stream) {}

void Transistor::print(std::ostream &stream) {}

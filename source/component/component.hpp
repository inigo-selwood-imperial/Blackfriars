#pragma once

#include <functional>
#include <memory>

#include "parse_buffer.hpp"

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

    // virtual void print(std::ostream &stream) = 0;

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

    template <typename PassiveType, char designator_prefix>
    static typename PassiveType::Pointer parse(Parse::Buffer &buffer);

    // template <typename PassiveType, char designator_prefix>
    // static typename void print(std::ostream &stream);

};

class Capacitor : public Passive,
        public ComponentProxy<Capacitor, Component::CAPACITOR> {

public:

    static Pointer parse(Parse::Buffer &buffer);

    // void print(std::ostream &stream) override;

};

class Inductor : public Passive,
        public ComponentProxy<Inductor, Component::INDUCTOR> {

public:

    static Pointer parse(Parse::Buffer &buffer);

    // void print(std::ostream &stream) override;

};

class Resistor : public Passive,
        public ComponentProxy<Resistor, Component::RESISTOR> {

public:

    static Pointer parse(Parse::Buffer &buffer);

    // void print(std::ostream &stream) override;

};

// ************************************************************** Semiconductors

class Semiconductor {};

class Diode : public Semiconductor,
        public ComponentProxy<Diode, Component::DIODE> {

public:

    static Pointer parse(Parse::Buffer &buffer);

    // void print(std::ostream &stream) override;

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

    // void print(std::ostream &stream) override;

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

    template <typename SourceType, char designator_prefix>
    static typename SourceType::Pointer parse(Parse::Buffer &buffer);

    // template <typename SourceType, char designator_prefix>
    // static typename void print(std::ostream &stream);

    Source() {
        amplitude = 0;
        cycles = 0;
        damping_factor = 0;
        delay = 0;
        frequency = 0;
        offset = 0;
        phase = 0;
    }

};

class CurrentSource : public Source,
        public ComponentProxy<CurrentSource, Component::CURRENT_SOURCE> {

public:

    static Pointer parse(Parse::Buffer &buffer);

    // void print(std::ostream &stream) override;

};

class VoltageSource : public Source,
        public ComponentProxy<VoltageSource, Component::VOLTAGE_SOURCE> {

public:

    static Pointer parse(Parse::Buffer &buffer);

    // void print(std::ostream &stream) override;

};

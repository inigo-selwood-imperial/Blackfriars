#pragma once

#include <functional>
#include <locale>
#include <memory>

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

// ************************************************************** Error handlers

struct ParseLogicError {};

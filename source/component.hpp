#pragma once

class Component {

    enum Type {
        VOLTAGE_SOURCE,
        CURRENT_SOURCE,

        DIODE,
        TRANSISTOR,

        RESISTOR,
        CAPACITOR,
        INDUCTOR
    };

    typedef std::shared_ptr<Term> Pointer;

    const Type type;

    template <typename DerivedType>
    static std::shared_ptr<DerivedType> cast(const Pointer &pointer);

    static Term::Pointer parse(ParseBuffer &buffer);

    Term(const Type &type) : type(type) {}

};

template <typename DerivedType, Component::Type type>
class ProxyComponent : public Component {

    typedef std::shared_ptr<DerivedType> Pointer;

    ProxyTerm() : Term(type) {}

};

class CurrentSource : public ProxyComponent<CurrentSource, CURRENT_SOURCE> {};
class VoltageSource : public ProxyComponent<VoltageSource, VOLTAGE_SOURCE> {};

class Diode : public ProxyComponent<Diode, DIODE> {};
class Transistor : public ProxyComponent<Transistor, TRANSISTOR> {};

class Capacitor : public ProxyComponent<Capacitor, CAPACITOR> {};
class Inductor : public ProxyComponent<Inductor, INDUCTOR> {};
class Resistor : public ProxyComponent<Resistor, RESISTOR> {};

CurrentSource::Pointer CurrentSource::parse(ParseBuffer &buffer);
VoltageSource::Pointer VoltageSource::parse(ParseBuffer &buffer);

Diode::Pointer Diode::parse(ParseBuffer &buffer);
Transistor::Pointer Transistor::parse(ParseBuffer &buffer);

Capacitor::Pointer Capacitor::parse(ParseBuffer &buffer);
Inductor::Pointer Inductor::parse(ParseBuffer &buffer);
Resistor::Pointer Resistor::parse(ParseBuffer &buffer);

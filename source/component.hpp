#pragma once

#include <memory>
#include <unordered_set>

#include "parse.hpp"

// ******************************************************************* Component

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

        const Type type;

        std::vector<std::string> nodes;

        std::string name;

        static bool is_symbol(const char &character) {
            return symbols.find(character) != symbols.end();
        }

        static std::shared_ptr<Component> parse(Parse::Buffer &buffer);

        static bool skip_designator(Parse::Buffer &buffer, const char &symbol) {
            if(buffer.skip_character(symbol) == false) {
                Log::error() << "Parse logic error, line " <<
                        buffer.get_position().line << std::endl;
                return false;
            }

            // If a name has given been to the component other than a plain old
            // integer, a '§' character is used to seperate them
            buffer.skip_string({-62, -89});

            return true;
        }

        Component(const Type &type) : type(type) {
            nodes.resize(2);
        }

    private:

        static const std::unordered_set<char> symbols;

    };

const std::unordered_set<char> Component::symbols = {'C', 'L', 'R', 'I', 'V',
        'D', 'Q'};

// ******************************************************** Passives definitions

class Passive {

    public:

    double value;

    template <typename PassiveType>
    static std::shared_ptr<PassiveType> parse(Parse::Buffer &buffer,
            const char &symbol) {

        if(Component::skip_designator(buffer, symbol) == false)
            return nullptr;

        auto passive = std::shared_ptr<PassiveType>(new PassiveType());

        std::vector<std::reference_wrapper<std::string>> tokens = {
            passive->name,
            passive->nodes[0],
            passive->nodes[1]
        };

        for(std::string &token : tokens) {
            buffer.skip_whitespace();
            token = Parse::token(buffer);
            if(token.empty())
                return nullptr;
        }

        buffer.skip_whitespace();
        try {
            passive->value = Parse::metric_value(buffer);
        }
        catch(...) {
            return nullptr;
        }

        return passive;
    }

};

struct Capacitor : public Passive, public Component {

    Capacitor() : Component(Component::CAPACITOR) {}

    static std::shared_ptr<Capacitor> parse(Parse::Buffer &buffer) {
        return Passive::parse<Capacitor>(buffer, 'C');
    }

};

struct Inductor : public Passive, public Component  {

    Inductor() : Component(Component::INDUCTOR) {}

    static std::shared_ptr<Inductor> parse(Parse::Buffer &buffer) {
        return Passive::parse<Inductor>(buffer, 'L');
    }

};

struct Resistor : public Passive, public Component  {

    Resistor() : Component(Component::RESISTOR) {}

    static std::shared_ptr<Resistor> parse(Parse::Buffer &buffer) {
        return Passive::parse<Resistor>(buffer, 'R');
    }

};

// ********************************************************* Sources definitions

class Source {

    public:

        struct Function {

            double amplitude;
            double cycle_count;
            double delay;
            double frequency;
            double offset;
            double phi;
            double theta;

            Function() {
                 amplitude = 0;
                 cycle_count = 0;
                 delay = 0;
                 frequency = 0;
                 offset = 0;
                 phi = 0;
                 theta = 0;
            }

            static std::shared_ptr<Function> parse(Parse::Buffer &buffer);

        };

        std::shared_ptr<Function> function;

        template <typename SourceType>
        static std::shared_ptr<SourceType> parse(Parse::Buffer &buffer,
                const char &symbol) {

            if(Component::skip_designator(buffer, symbol) == false)
                return nullptr;

            auto source = std::shared_ptr<SourceType>(new SourceType());

            std::vector<std::reference_wrapper<std::string>> tokens = {
                source->name,
                source->nodes[0],
                source->nodes[1]
            };

            for(std::string &token : tokens) {
                buffer.skip_whitespace(Parse::SPACES);
                token = Parse::token(buffer);
                if(token.empty())
                    return nullptr;
            }

            buffer.skip_whitespace();
            auto function = Function::parse(buffer);
            if(function == nullptr)
                return nullptr;
            source->function = function;

            return source;
        }

        double value(const double &time) {
            const double omega = 2 * 3.14159265359 * function->frequency *
                    (time - function->delay);
            return function->amplitude * std::sin(omega + function->theta) +
                    function->offset;
        }

    };

struct VoltageSource : public Source, public Component {

    VoltageSource() : Component(Component::VOLTAGE_SOURCE) {}

    static std::shared_ptr<VoltageSource> parse(Parse::Buffer &buffer) {
        return Source::parse<VoltageSource>(buffer, 'V');
    }

};

struct CurrentSource : public Source, public Component {

    CurrentSource() : Component(Component::CURRENT_SOURCE) {}

    static std::shared_ptr<CurrentSource> parse(Parse::Buffer &buffer) {
        return Source::parse<CurrentSource>(buffer, 'I');
    }

};

// ************************************************** Semiconductors definitions

struct Diode : public Component{

    Diode() : Component(Component::DIODE) {}

    static std::shared_ptr<Diode> parse(Parse::Buffer &buffer) {
        if(Component::skip_designator(buffer, 'D') == false)
            return nullptr;

        return nullptr;
    }

};

struct Transistor : public Component {

    Transistor() : Component(Component::TRANSISTOR) {
        nodes.resize(3);
    }

    static std::shared_ptr<Transistor> parse(Parse::Buffer &buffer) {
        if(Component::skip_designator(buffer, 'Q') == false)
            return nullptr;
        return nullptr;
    }

};

// Source parse thing
std::shared_ptr<Source::Function> Source::Function::parse(
        Parse::Buffer &buffer) {

    auto function = std::shared_ptr<Function>(new Function());

    // The source can have either a constant value, or a sinusoidal function
    // (for the purposes of this application). Delegate to the relevant
    // case
    // NOTE: For the purposes of a constant value, the source is treated as
    // a sinusoidal one with an amplitude and frequency of zero, and a constant
    // offset
    buffer.skip_whitespace();
    if(buffer.skip_string("SINE(")) {

        // As much as I would like to alphabetize these references, the gods of
        // LTSpice format have dictated that they appear in this order
        std::vector<std::reference_wrapper<double>> fields = {
            function->offset,
            function->amplitude,
            function->frequency,
            function->delay,
            function->theta,
            function->phi,
            function->cycle_count
        };

        // The number of fields in any one sine function varies. Only the ones
        // set will have values, those preceding it will be set to 0, and any
        // that might have come after will be omitted. Therefore, we want to
        // parse fields up until a non-value field is encountered (a ')'
        // character, if the text is formatted properly)
        for(double &field : fields) {
            buffer.skip_whitespace();

            const auto character = buffer.get_current();
            if((character >= '0' && character <= '9') ||
                    character == '.' || character == '-') {

                try {
                    field = Parse::metric_value(buffer);
                }
                catch(...) {
                    Log::error() << "Unexpected character in source sine "
                            "function, " << buffer.get_position() << std::endl;
                    return nullptr;
                }
            }
            else
                break;
        }

        // Skip the closing bracket
        if(buffer.skip_character(')') == false) {
            Log::error() << "Malformed source sine function, line " <<
                    buffer.get_position().line << std::endl;
            return nullptr;
        }
    }

    // If the source is not sinusoidal, we want to set its constant value as
    // the source's DC offset.
    else
        function->offset = Parse::metric_value(buffer);

    return function;
}

std::shared_ptr<Component> Component::parse(Parse::Buffer &buffer) {
    switch(buffer.get_current()) {
        case 'C':
            return Capacitor::parse(buffer);
        case 'L':
            return Inductor::parse(buffer);
        case 'R':
            return Resistor::parse(buffer);
        case 'I':
            return CurrentSource::parse(buffer);
        case 'V':
            return VoltageSource::parse(buffer);
        default:
            return nullptr;
    }
}

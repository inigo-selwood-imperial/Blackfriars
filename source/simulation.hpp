#pragma once

#include <memory>
#include <vector>

#include "component.hpp"
#include "parse.hpp"

class Simulation {

public:

    struct Operation {

        double start_time;
        double stop_time;
        double time_step;

        static std::shared_ptr<Operation> parse(Parse::Buffer &buffer);

        Operation();

    };

    static std::shared_ptr<Simulation> parse(const std::string &netlist);

    bool run(std::fstream &stream);

private:

    std::vector<std::shared_ptr<Component>> components;

    std::shared_ptr<Operation> operation;

};

// ************************************************************ Parse operations

std::shared_ptr<Simulation::Operation> Simulation::Operation::parse(
        Parse::Buffer &buffer) {

    if(buffer.skip_string(".tran") == false)
        return nullptr;

    auto operation = std::shared_ptr<Operation>(new Operation());

    buffer.skip_whitespace();
    buffer.skip_character('0');

    std::vector<std::reference_wrapper<double>> fields = {
        operation->stop_time,
        operation->start_time,
        operation->time_step
    };

    for(double &field : fields) {
        buffer.skip_whitespace();

        const auto character = buffer.get_current();
        if((character >= '0' && character <= '9') ||
                character == '.' || character == '-') {

            try {
                field = Parse::metric_value(buffer);
            }
            catch(...) {
                Log::error() << "Unexpected character in operation "
                        "specification, " << buffer.get_position() <<
                        std::endl;
                return nullptr;
            }
            buffer.skip_character('s');
        }
        else
            break;
    }

    return operation;
}

std::shared_ptr<Simulation> Simulation::parse(const std::string &netlist) {
    auto simulation = std::shared_ptr<Simulation>(new Simulation());

    Parse::Buffer buffer(netlist);
    while(true) {
        buffer.skip_whitespace();

        // This condition shouldn't occur if the netlist is written right, but
        // it'll prevent infinite loops
        if(buffer.end_reached())
            break;

        // Parse SPICE command
        else if(buffer.get_current() == '.') {
            Log::debug() << "Parsing command" << std::endl;

            if(buffer.skip_string(".end"))
                break;
            else if(buffer.get_string(".tran")) {
                auto operation = Operation::parse(buffer);
                if(operation == nullptr) {
                    Log::error() << "Malformed operation specification, "
                            "line " << buffer.get_position().line << std::endl;
                    return nullptr;
                }
                simulation->operation = operation;
            }

            // There are other SPICE commands, but none that apply here -- so
            // ignore them
            else
                buffer.skip_line();
        }

        // Parse component
        else if(Component::is_symbol(buffer.get_current())) {
            Log::debug() << "Parsing component" << std::endl;

            const auto component = Component::parse(buffer);
            if(component == nullptr) {
                Log::error() << "Malformed component specification, line " <<
                        buffer.get_position().line << std::endl;
                return nullptr;
            }
            else
                simulation->components.push_back(component);
        }

        // If none of the above apply, there's a formatting error somewhere in
        // the netlist
        else {
            Log::error() << "Unrecognized syntax, line " <<
                    buffer.get_position().line << std::endl;
            return nullptr;
        }

        // All of the above cases should bring the buffer to the end of the
        // line, otherwise, there's a formatting error
        if(buffer.skip_character('\n') == false) {
            Log::error() << "Syntax error, line " <<
                    buffer.get_position().line << std::endl;
            return nullptr;
        }
    }

    // Check there were components in the netlist
    if(simulation->components.empty()) {
        Log::error() << "No components in netlist" << std::endl;
        return nullptr;
    }

    // Check an operation was specified in the netlist
    else if(simulation->operation == nullptr) {
        Log::error() << "No operation in netlist" << std::endl;
        return nullptr;
    }

    else
        return simulation;
}

// **************************************************************** Run function

bool Simulation::run(std::fstream &stream) {
    if(components.empty()) {
        Log::error() << "Can't run simulation without any components" <<
                std::endl;
        return false;
    }

    else if(operation == nullptr) {
        Log::error() << "Can't run simulation without an operation "
                "specified" << std::endl;
        return false;
    }

    for(double time = operation->start_time; time < operation->stop_time;
            time += operation->time_step) {

        stream << time;

        stream << std::endl;
    }

    return true;
}

// **************************************************************** Constructors

Simulation::Operation::Operation() {
    start_time = 0;
    stop_time = 0;
    time_step = 0;
}

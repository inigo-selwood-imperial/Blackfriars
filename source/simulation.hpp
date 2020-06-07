#pragma once

#include <map>
#include <memory>
#include <vector>

#include <cmath>

#include "component.hpp"
#include "parse.hpp"
#include "matrix.hpp"

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

    static inline void populate_instances(const std::vector<std::shared_ptr<Component>> &components) {
        std::map<Component::Type, unsigned int> instances;
        for(auto &component : components) {
            auto &instance_count = instances[component->type];
            component->instance = instance_count;
            instance_count += 1;
        }
    }

    static inline std::map<std::string, unsigned int> isolate_nodes(const std::vector<std::shared_ptr<Component>> &components) {
        std::map<std::string, unsigned int> nodes;
        for(const auto &component : components) {
            for(unsigned int index = 0; index < component->nodes.size(); index += 1)
                nodes[component->nodes[index]] += 1;
        }
        return nodes;
    }

    static inline unsigned int count_voltage_sources(
            const std::vector<std::shared_ptr<Component>> &components);

    static inline unsigned int count_current_sources(
            const std::vector<std::shared_ptr<Component>> &components) {

        unsigned int result = 0;
        for(const auto &component : components) {
            if(component->type == Component::CAPACITOR)
                result += 1;
        }
        return result;
    }

    std::vector<std::shared_ptr<Component>> components;

    std::shared_ptr<Operation> operation;

};

// *************************************************************** Count helpers

inline unsigned int Simulation::count_voltage_sources(
        const std::vector<std::shared_ptr<Component>> &components) {

    unsigned int result = 0;
    for(const auto &component : components) {
        if(component->type == Component::VOLTAGE_SOURCE)
            result += 1;
    }
    return result;
}

// ******************************************************** Serialization helper



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

    // Assign each component a unique instance number
    populate_instances(simulation->components);

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

    auto nodes = isolate_nodes(components);

    const auto node_count = nodes.size();
    const auto voltage_source_count = count_voltage_sources(components);
    const auto current_source_count = count_current_sources(components);

    const auto size = node_count + voltage_source_count + current_source_count;

    Matrix conductances(size, size);
    Matrix constants(1, size);

    for(double time = operation->start_time; time < operation->stop_time;
            time += operation->time_step) {

        for(const auto &component : components) {

            // Handle capacitors
            if(component->type == Component::CAPACITOR) {
                const auto capacitor = Component::cast<Capacitor>(component);

                const auto node_0 = nodes[capacitor->nodes[0]];
                const auto node_1 = nodes[capacitor->nodes[1]];

                const unsigned int column = node_count + voltage_source_count +
                        capacitor->instance - 1;
                const auto voltage = capacitor->voltage(time);
                if(node_0) {
                    conductances(node_0 - 1, column) = 1;
                    constants(node_count + node_0 - 2, 0) = voltage;
                }
                if(node_1) {
                    conductances(node_1 - 1, column) = -1;
                    constants(node_count + node_1 - 2, 0) = -voltage;
                }
            }

            // Handle inductors
            else if(component->type == Component::INDUCTOR) {
                const auto inductor = Component::cast<Inductor>(component);

                const auto node_0 = nodes[inductor->nodes[0]];
                const auto node_1 = nodes[inductor->nodes[1]];

                const auto current = inductor->current(time);
                if(node_0)
                    constants(node_0 - 1, 0) = current;
                if(node_1)
                    constants(node_1 - 1, 0) = -current;
            }

            // Handle resistors
            else if(component->type == Component::RESISTOR) {
                const auto resistor = Component::cast<Resistor>(component);

                const auto node_0 = nodes[resistor->nodes[0]];
                const auto node_1 = nodes[resistor->nodes[1]];

                if(node_0)
                    conductances(node_0 - 1, node_0 - 1) += resistor->value;
                if(node_1)
                    conductances(node_1 - 1, node_1 - 1) += resistor->value;

                if(node_0 && node_1) {
                    conductances(node_0 - 1, node_1 - 1) += 1 / resistor->value;
                    conductances(node_1 - 1, node_0 - 1) += 1 / resistor->value;
                }
            }

            // Handle current sources
            else if(component->type == Component::CURRENT_SOURCE) {
                const auto current_source =
                        Component::cast<CurrentSource>(component);

                const auto node_0 = nodes[current_source->nodes[0]];
                const auto node_1 = nodes[current_source->nodes[1]];

                const auto value = current_source->value(time);
                if(node_0)
                    constants(node_0 - 1, 0) = value;
                if(node_1)
                    constants(node_1 - 1, 0) = value;
            }

            // Handle voltage sources
            else if(component->type == Component::VOLTAGE_SOURCE) {
                const auto voltage_source =
                        Component::cast<VoltageSource>(component);

                const auto node_0 = nodes[voltage_source->nodes[0]];
                const auto node_1 = nodes[voltage_source->nodes[1]];

                const unsigned int column = node_count +
                        voltage_source->instance;
                const auto value = voltage_source->value(time);
                if(node_0) {
                    conductances(node_0 - 1, column) = 1;
                    constants(node_count + node_0 - 2, 0) = value;
                }
                if(node_1) {
                    conductances(node_1 - 1, column) = -1;
                    constants(node_count + node_1 - 2, 0) = value;
                }
            }
        }

        // Transpose the voltage source sign multipliers
        for(unsigned int row = 0; row < node_count; row += 1) {
            for(unsigned int column = node_count; column < size; column += 1) {
                conductances(column, row) = conductances(row, column);
            }
        }

        // Negate and reciprocate the off-diagonal conductance values
        for(unsigned int row = 0; row < node_count; row += 1) {
            for(unsigned int column = 0; column < node_count; column += 1) {
                if(row == column)
                    continue;

                if(conductances(row, column))
                    conductances(row, column) = -1 / conductances(row, column);
            }
        }

        std::cout << conductances << std::endl;
        std::cout << constants << std::endl;

        break;
    }

    return true;
}

// **************************************************************** Constructors

Simulation::Operation::Operation() {
    start_time = 0;
    stop_time = 0;
    time_step = 0;
}

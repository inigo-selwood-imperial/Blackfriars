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

    std::vector<std::shared_ptr<Component>> components;

    std::shared_ptr<Operation> operation;

    static inline void assign_instances(
            const std::vector<std::shared_ptr<Component>> &components);
    static inline void assign_nodes(
            const std::vector<std::shared_ptr<Component>> &components);
};

inline void Simulation::assign_nodes(
        const std::vector<std::shared_ptr<Component>> &components) {

    std::map<std::string, unsigned int> nodes = {{"0", 0}};
    unsigned int last_index = 0;
    for(auto &component : components) {
        for(unsigned int index = 0;
                index < ((component->type == Component::TRANSISTOR) ? 3 : 2);
                index += 1) {

            const auto &name = component->node_names[index];
            if(nodes.find(name) == nodes.end()) {
                last_index += 1;
                nodes[name] = last_index;
                component->node_indices[index] = last_index;
            }
            else
                component->node_indices[index] = nodes[name];
        }
    }
}

inline void Simulation::assign_instances(
        const std::vector<std::shared_ptr<Component>> &components) {

    std::map<Component::Type, unsigned int> instances;
    for(auto &component : components) {
        auto &instance_count = instances[component->type];
        component->instance = instance_count;
        instance_count += 1;
    }
}

std::shared_ptr<Simulation::Operation> Simulation::Operation::parse(
        Parse::Buffer &buffer) {

    if(buffer.skip_string(".tran") == false)
        return nullptr;

    auto operation = std::shared_ptr<Operation>(new Operation());

    buffer.skip_whitespace();
    if(buffer.skip_string("0 ") == false) {
        Log::error() << "Too few arguments for operation specification, "
                "line " << buffer.get_position().line << std::endl;
        return nullptr;
    }

    std::vector<std::reference_wrapper<double>> fields = {
        operation->stop_time,
        operation->start_time,
        operation->time_step
    };

    for(double &field : fields) {
        buffer.skip_whitespace(Parse::SPACES | Parse::TABS);

        if(Parse::is_number(buffer.get_current())) {

            try {
                field = Parse::metric_value(buffer);
            }
            catch(...) {
                Log::error() << "Unexpected character in operation "
                        "specification, line " << buffer.get_position().line <<
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

    // Check there were components in the netlist
    if(simulation->components.empty()) {
        Log::error() << "No components in netlist" << std::endl;
        return nullptr;
    }

    // Give each component a unique identifier based on its type, and assign
    // each of its nodes an index
    assign_nodes(simulation->components);
    assign_instances(simulation->components);

    // Check an operation was specified in the netlist
    if(simulation->operation == nullptr) {
        Log::error() << "No operation in netlist" << std::endl;
        return nullptr;
    }

    return simulation;
}

bool Simulation::run(std::fstream &stream) {

    // Check there are components to simulate
    if(components.empty()) {
        Log::error() << "Can't run simulation without any components" <<
                std::endl;
        return false;
    }

    // Check there's an operation specified
    else if(operation == nullptr) {
        Log::error() << "Can't run simulation without an operation "
                "specified" << std::endl;
        return false;
    }

    unsigned int node_count = 0;
    unsigned int voltage_source_count = 0;
    unsigned int capacitor_count = 0;

    for(const auto &component : components) {
        for(unsigned int index = 0;
                index < ((component->type == Component::TRANSISTOR) ? 3 : 2);
                index += 1) {

            node_count = std::max(node_count, component->node_indices[index]);
        }

        if(component->type == Component::CAPACITOR)
            capacitor_count += 1;
        else if(component->type == Component::VOLTAGE_SOURCE)
            voltage_source_count += 1;
    }

    unsigned int size = node_count + voltage_source_count + capacitor_count;

    Matrix conductances(size, size);
    Matrix constants(1, size);
    Matrix result(1, size);

    for(double time = operation->start_time; time < operation->stop_time;
            time += operation->time_step) {

        for(auto &component : components) {
            component->tabulate(conductances, constants, result,
                    node_count, voltage_source_count, time);
        }

        for(unsigned int row = 0; row < node_count; row += 1) {
            for(unsigned int column = 0; column < node_count; column += 1) {
                if(row == column)
                    continue;

                if(conductances(row, column))
                    conductances(row, column) = -1 / conductances(row, column);
            }
        }

        for(unsigned int row = 0; row < node_count; row += 1) {
            for(unsigned int column = node_count; column < size; column += 1)
                conductances(column, row) = conductances(row, column);
        }

        std::cout << conductances << std::endl;
        std::cout << constants << std::endl;

        std::cout << conductances.inverse() << std::endl;

        result = conductances.inverse();
        std::cout << (result * constants) << std::endl;

        break;
    }

    return true;
}

Simulation::Operation::Operation() {
    start_time = 0;
    stop_time = 0;
    time_step = 0;
}

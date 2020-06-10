#pragma once

#include "parse_buffer.hpp"

#include "operation.hpp"
#include "schematic.hpp"

class Simulation {

public:

    std::shared_ptr<Operation> operation;

    Schematic schematic;

    static std::shared_ptr<Simulation> create(const std::string &specification);

    bool run(std::ostream &stream);

};

// Create a simulation from an LTSpice specification
std::shared_ptr<Simulation> Specification::create(
        const std::string &specification) {

    auto simulation = std::shared_ptr<Simulation>(new Simulation());

    Parse::Buffer buffer(specification);
    while(true) {
        if(buffer.end_reached())
            break;

        const char character = buffer.get_current();

        // Parse components
        if(character >= 'A' && character <= 'Z') {
            const auto component = Component::parse(buffer);
            if(component == nullptr) {
                std::cerr << "Couldn't parse component" << std::endl;
                return nullptr;
            }

            simulation->schematic.add_component(component);
        }

        // Parse commands
        else if(character == '.') {
            if(Operation::is_definition(buffer)) {
                const auto operation = Operation::parse(buffer);
                if(operation == nullptr) {
                    std::cerr << "Couldn't parse operation" << std::endl;
                    return nullptr;
                }

                simulation->operation = operation;
            }
            else
                buffer.skip_line();
        }
    }

    return simulation;
}

// Run the simulation
bool Simulation::run(std::ostream &stream) {
    if(operation == nullptr) {
        std::cerr << "No operation specified" << std::endl;
        return false;
    }

    return operation->run(schematic, stream);
}

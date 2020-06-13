#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <ostream>

#include "components/templates/component.hpp"

#include "operations/operation.hpp"

#include "schematic.hpp"

#include "operations/transient.hpp"

#include "components/resistor.hpp"
#include "components/capacitor.hpp"
#include "components/inductor.hpp"

#include "components/current_source.hpp"
#include "components/voltage_source.hpp"

class Simulation {

public:

    std::shared_ptr<Operation> operation;

    Schematic schematic;

    static std::shared_ptr<Simulation> parse(const std::string &specification);

    static std::shared_ptr<Component> parse_component(
            TextBuffer &buffer);

    bool run(std::ostream &stream);

};

// Parse a simulation from a SPICE netlist
std::shared_ptr<Simulation> Simulation::parse(
        const std::string &specification) {

    // Create a new simulation pointer
    auto simulation = std::shared_ptr<Simulation>(new Simulation());

    TextBuffer buffer(specification);
    while(true) {

        // Stop if the end of the text's been reached
        buffer.skip_whitespace();
        if(buffer.end_reached())
            break;

        const auto character = buffer.get_character();

        // Skip comments
        if(character == '*')
            buffer.skip_line();

        // Handle commands
        else if(character == '.') {

            // Parse transient operation specifications
            if(buffer.get_string() == ".tran") {
                const auto transient = Transient::parse(buffer);
                if(transient == nullptr) {
                    std::cerr << "Couldn't parse transient operation, line " <<
                            buffer.get_line_number() << std::endl;
                    return nullptr;
                }

                simulation->operation = transient;
            }

            // If it's not an operation, we can ignore it for the purposes of
            // this application
            else
                buffer.skip_line();
        }

        // Handle component definitions
        else if(character >= 'A' && character <= 'Z') {
            const auto component = parse_component(buffer);
            if(component == nullptr) {
                std::cerr << "Error parsing component, line " <<
                        buffer.get_line_number() << std::endl;
                return nullptr;
            }

            simulation->schematic.add_component(component);
        }

        // Check that the current line/lines have been fully parsed
        if(buffer.end_reached() == false && buffer.skip_character('\n') ==
                false) {

            std::cerr << "Syntax error, line " << buffer.get_line_number() <<
                    std::endl;
            std::cerr << buffer.get_character() << std::endl;
            return nullptr;
        }
    }

    // Check there were components found in the file
    bool failed = false;
    if(simulation->schematic.empty()) {
        std::cerr << "No components found in file" << std::endl;
        failed = true;
    }

    // Check there was an operation specified in the file
    if(simulation->operation == nullptr) {
        std::cerr << "No operation specified in file" << std::endl;
        failed = true;
    }

    if(failed)
        return nullptr;

    return simulation;

}

// Parses a component definition
std::shared_ptr<Component>  Simulation::parse_component(TextBuffer &buffer) {
    switch(buffer.get_character()) {
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
            std::cerr << "Couldn't identify component type" << std::endl;
            return nullptr;
    }
}

// Run the simulation provided
bool Simulation::run(std::ostream &stream) {

    // Check there's an operation specified
    bool failed = false;
    if(operation == nullptr) {
        std::cerr << "No operation in simulation" << std::endl;
        failed = true;
    }

    // Check there are components to simulate
    if(schematic.empty()) {
        std::cerr << "No components in simulation schematic" << std::endl;
        failed = true;
    }

    if(failed)
        return false;

    // Run the simulation
    if(operation->run(schematic, stream) == false) {
        std::cerr << "Operation failed" << std::endl;
        return false;
    }

    return true;
}

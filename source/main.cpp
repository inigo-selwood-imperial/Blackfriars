#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <fstream>

#include "parse_buffer.hpp"

#include "operation.hpp"
#include "schematic.hpp"

class Simulation {

public:

    std::shared_ptr<Operation> operation;

    Schematic schematic;

    static std::shared_ptr<Simulation> create(const std::string &specification);

};

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

int main(int argument_count, char *argument_vector[]) {

    // Place arguments into a vector
    argument_count -= 1;
    std::vector<std::string> arguments(argument_count);
    for(unsigned int index = 0; index < argument_count; index += 1)
        arguments[index] = argument_vector[index + 1];

    // Parse command line arguments
    std::string input_file_name = "";
    std::string output_file_name = "output.csv";
    for(unsigned int index = 0; index < argument_count; index += 1) {

        // Handle output file name specifications
        if(arguments[index] == "-output") {
            if(index + 1 >= argument_count || arguments[index + 1][0] == '-') {
                std::cerr << "Output file name set flag '-output' encountered, "
                        "but no file name provided." << std::endl;
            }

            // Skip the file name specified
            output_file_name = arguments[index + 1];
            index += 1;
        }

        // The first argument is expected to be the input file
        else if(index == 0) {
            if(arguments[index][0] == '-') {
                std::cerr << "First argument can't be an option flag" <<
                        std::endl;
                return -1;
            }

            input_file_name = arguments[index];
        }

        // If it's neither the first argument or an option flag, then the
        // argument's not supported
        else {
            std::cerr << "Couldn't recognize argument '" << arguments[index] <<
                    "'" << std::endl;
            return -1;
        }
    }

    // Open input file
    std::ifstream input_file(input_file_name);
    if(input_file.is_open() == false) {
        std::cerr << "Couldn't open input file '" << input_file << "'" <<
                std::endl;
        return -1;
    }

    // Load input file into string
    std::string specification((std::istreambuf_iterator<char>(input_file)),
                     std::istreambuf_iterator<char>());

    // Create simulation
    auto simulation = Simulation::create(specification);
    if(simulation == nullptr) {
        std::cerr << "Couldn't create simulation" << std::endl;
        return -1;
    }

    // Open output file
    std::ofstream output_file(output_file_name);
    if(output_file.is_open() == false) {
        if(output_file_name = "output.csv")
            std::cerr << "Couldn't create output file" << std::endl;
        else {
            std::cerr << "Couldn't open/create output file '" << output_file <<
                    "'" << std::endl;
        }
        return -1;
    }

    // Run simulation
    if(simulation->run(output_file) == false) {
        std::cerr << "Couldn't run simulation" << std::endl;
        return -1;
    }

    return 0;
}

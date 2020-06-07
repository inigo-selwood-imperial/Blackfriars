#include <iostream>
#include <string>
#include <vector>

#include "log.hpp"
#include "simulation.hpp"

static inline std::string load_file(const std::string &file_name) {
    std::ifstream stream(file_name);
    if(stream.is_open() == false)
        return "";

    return std::string((std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>());
}

static inline std::vector<std::string> wrap_arguments(int &argument_count,
        char *argument_vector[]) {

    std::vector<std::string> arguments;
    argument_count -= 1;
    for(int index = 0; index < argument_count; index += 1)
        arguments.push_back(argument_vector[index + 1]);
    return arguments;
}

int main(int argument_count, char *argument_vector[]) {

    // Wrap arguments in a more idiomatic container
    const auto arguments = wrap_arguments(argument_count, argument_vector);
    if(arguments.size() != 2) {
        Log::error() << "No arguments provided" << std::endl;
        return -1;
    }

    // Load the netlist
    const auto netlist = load_file(arguments[0]);
    if(netlist.empty()) {
        Log::error() << "Netlist file either wasn't provided, didn't exist, "
                "or was empty" << std::endl;
        return -1;
    }

    // Parse the netlist
    auto simulation = Simulation::parse(netlist);
    if(simulation == nullptr) {
        Log::error() << "Couldn't parse simulation" << std::endl;
        return -1;
    }

    // Print the results of the simulation to the output file
    std::fstream output_file(arguments[1], std::fstream::out);
    if(output_file.is_open() == false) {
        Log::error() << "Output file either couldn't be created, or isn't "
                "accessible" << std::endl;
        return -1;
    }

    if(simulation->run(output_file) == false) {
        Log::error() << "Error occured during simulation" << std::endl;
        return -1;
    }

    return 0;
}

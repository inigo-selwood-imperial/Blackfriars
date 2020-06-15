#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include <ctime>

#include "simulation.hpp"

int main(int argument_count, char *argument_vector[]) {

    // Start timer
    std::clock_t start;
    double duration;
    start = std::clock();

    std::vector<std::string> arguments;
    for(unsigned int index = 1; index < argument_count; index += 1)
        arguments.push_back(argument_vector[index]);
    argument_count -= 1;

    // Parse the command line arguments
    std::string input_file_name;
    std::string output_file_name;
    for(unsigned int index = 0; index < argument_count; index += 1) {

        // Parse output file flag
        if(arguments[index] == "-output") {
            if(index + 1 >= argument_count) {
                std::cerr << "'-output' flag present in arguments, but wasn't "
                        "followed by a filename" << std::endl;
                return -1;
            }

            output_file_name = arguments[index + 1];
            index += 1;
        }

        // Parse input file name
        else if(input_file_name.empty())
            input_file_name = arguments[index];

        // Failing all else, report an error
        else {
            std::cerr << "Unrecognized argument '" << arguments[index] << "'" <<
                    std::endl;
            return -1;
        }
    }

    // Check an input file's been specified
    if(input_file_name.empty()) {
        std::cerr << "No input file specified" << std::endl;
        return -1;
    }

    // Open the input stream
    std::ifstream input_file(input_file_name);
    if(input_file.is_open() == false) {
        std::cerr << "Couldn't open input file '" << input_file_name << "'" <<
                std::endl;
        return -1;
    }

    // Load input file into string, and create an instance of the simulation
    // class
    std::string specification((std::istreambuf_iterator<char>(input_file)),
                     std::istreambuf_iterator<char>());
    auto simulation = Simulation::parse(specification);
    if(simulation == nullptr) {
        std::cerr << "Failed to create simulation" << std::endl;
        return -1;
    }

    // Open/assign the output method
    const bool output_file_specified = output_file_name.empty() == false;
    std::ofstream output_file;
    if(output_file_specified) {
        output_file = std::ofstream(output_file_name);
        if(output_file.is_open() == false) {
            std::cerr << "Couldn't open/create output file '" <<
                    output_file_name << "'" << std::endl;
            return -1;
        }
    }
    std::ostream &stream = output_file_specified ? output_file : std::cout;

    // Run the simulation
    if(simulation->run(stream) == false) {
        std::cerr << "Failed to run simulation" << std::endl;
        return -1;
    }

    // Stop timer
    duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
    std::cout << "Process took: " << duration << " microseconds" << std::endl;

    return 0;
}

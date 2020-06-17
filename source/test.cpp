#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include <ctime>

#include "simulation.hpp"

int main(int argument_count, char *argument_vector[]) {

    std::cout << "node count, time per iteration, shortest time, greatest_time" << std::endl;

    std::string netlist_base = "V1 N1 0 5\n";
    for(unsigned int node_count = 1; node_count < 15; node_count += 1) {
        std::string netlist = netlist_base;
        for(unsigned int index = 1; index < node_count; index += 1) {
            netlist += "R" + std::to_string(index) + " N" + std::to_string(index) + " N" + std::to_string(index + 1) + " 10\n";
        }
        netlist += "R" + std::to_string(node_count) + " N" + std::to_string(node_count) + " 0 10\n";
        netlist += ".tran 0.1\n";

        // Start timer

        std::vector<double> times;

        // Load input file into string, and create an instance of the simulation
        // class
        auto simulation = Simulation::parse(netlist);
        if(simulation == nullptr) {
            std::cerr << "Failed to create simulation" << std::endl;
            return -1;
        }

        const unsigned int iterations = 10;
        for(unsigned int iteration = 0; iteration < iterations; iteration += 1) {

            std::clock_t start;
            double duration;
            start = std::clock();

        	// Run the simulation
            if(simulation->run(nullptr) == false) {
                std::cerr << "Failed to run simulation" << std::endl;
                return -1;
            }

            times.push_back((std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000));
        }

        double shortest_time = 10000000;
        double greatest_time = 0;
        double average = 0;
        for(const auto &time : times) {
            shortest_time = std::min(shortest_time, time);
            greatest_time = std::max(greatest_time, time);
            average += time;
        }
        average /= iterations;
        std::cout << node_count << ", " << average << ", " << shortest_time << ", " << greatest_time << std::endl;
    }

    return 0;
}

#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "../parse.hpp"
#include "../text_buffer.hpp"

#include "operation.hpp"

#include "../matrix.hpp"

class Transient : public std::enable_shared_from_this<Transient>,
        public Operation {

private:

    std::map<std::string, unsigned int> node_indices;
    std::map<std::string, unsigned int> voltage_source_indices;

    std::map<unsigned int, std::array<double, 3>> node_voltages;
    std::map<unsigned int, std::array<double, 3>> voltage_source_currents;

    std::map<std::array<unsigned int, 2>, double> resistance_values;
    std::map<std::array<unsigned int, 2>, double> current_source_values;
    std::map<std::array<unsigned int, 3>, double> voltage_source_values;

    void update_values(const Matrix &result) {
        for(auto &pair : node_voltages) {
            auto &integral = pair.second[0];
            auto &previous_value = pair.second[1];
            auto &present_value = pair.second[2];

            const auto node_index = pair.first;

            integral += ((present_value + previous_value) / 2) * time_step;
            previous_value = present_value;
            present_value = result(node_index, 0);
        }

        const unsigned int node_count = node_indices.size();
        for(auto &pair : voltage_source_currents) {
            auto &integral = pair.second[0];
            auto &previous_value = pair.second[1];
            auto &present_value = pair.second[2];

            const auto voltage_source_index = pair.first;

            integral += ((present_value + previous_value) / 2) * time_step;
            previous_value = present_value;
            present_value = result(node_count + voltage_source_index, 0);
        }
    }

    unsigned int get_node_index(const std::string &name) {
        auto &index = node_indices[name];
        if(index == 0 && name != "0")
            index = node_indices.size() - 1;
        return index;
    }

    unsigned int get_voltage_source_index(const std::string &name) {
        auto &index = voltage_source_indices[name];
        if(index == 0 && voltage_source_indices.size())
            index = voltage_source_indices.size() - 1;
        return index;
    }

    inline Matrix create_constants_matrix() {
        const unsigned int node_count = node_indices.size();
        const unsigned int voltage_source_count = voltage_source_indices.size();
        const unsigned int size = node_count + voltage_source_count;

        Matrix constants(1, node_count + voltage_source_count);

        // Handle current sources
        for(const auto &pair : current_source_values) {
            const auto node_one = pair.first[0];
            const auto node_two = pair.first[1];
            const auto value = pair.second;

            if(node_one)
                constants(node_one - 1, 0) += value;
            if(node_two)
                constants(node_two - 1, 0) -= value;
        }

        // Handle voltage sources
        for(const auto &pair : voltage_source_values) {
            const auto index = pair.first[2];
            const auto value = pair.second;

            constants(node_count + index, 0) += value;
        }

        return constants;
    }

    inline Matrix create_conductance_matrix() {
        const unsigned int node_count = node_indices.size();
        const unsigned int voltage_source_count = voltage_source_indices.size();
        const unsigned int size = node_count + voltage_source_count;

        Matrix conductances(size, size);

        // Handle resistances
        for(const auto &pair : resistance_values) {
            const auto node_one = pair.first[0];
            const auto node_two = pair.first[1];
            const auto value = pair.second;

            if(node_one)
                conductances(node_one - 1, node_one - 1) += 1 / value;
            if(node_two)
                conductances(node_two - 1, node_two - 1) += 1 / value;
            if(node_one && node_two) {
                conductances(node_one - 1, node_two - 1) += value;
                conductances(node_two - 1, node_one - 1) += value;
            }
        }

        // Take the reciprocal of the resistances
        for(unsigned int row = 0; row < node_count; row += 1) {
            for(unsigned int column = 0; column < node_count; column += 1) {
                if(row == column)
                    continue;

                conductances(row, column) = -1 / conductances(row, column);
            }
        }

        // Place voltage source signs
        for(const auto &pair : voltage_source_values) {
            const auto node_one = pair.first[0];
            const auto node_two = pair.first[1];
            const auto index = pair.first[2];

            const unsigned int column = node_count + index;
            if(node_one) {
                conductances(node_one - 1, column) = -1;
                conductances(column, node_one - 1) = -1;
            }
            if(node_two) {
                conductances(node_two - 1, column) = 1;
                conductances(column, node_two - 1) = 1;
            }
        }

        return conductances;
    }

public:

    double start_time;
    double stop_time;
    double time_step;

    static std::shared_ptr<Transient> parse(TextBuffer &buffer);

    void add_voltage(const std::string &node_one, const std::string &node_two,
            const std::string &name, const double &value);
    void add_current(const std::string &node_one, const std::string &node_two,
            const double &value);
    void add_resistance(const std::string &node_one,
            const std::string &node_two, const double &value);

    double current_integral(const std::string &name);
    double voltage_integral(const std::string &node_one,
            const std::string &node_two);

    bool run(Schematic &schematic, const std::ostream &stream) override;

};

// Parses a '.tran' SPICE DC analysis transient specification
// Returns a null-pointer in case of error
std::shared_ptr<Transient> Transient::parse(TextBuffer &buffer) {

    auto transient = std::shared_ptr<Transient>(new Transient());

    // Check the function hasn't been called in error
    if(buffer.skip_string(".tran") == false) {
        std::cerr << "Transient parse function called when definition is not "
                "that of a transient function" << std::endl;
        return nullptr;
    }

    // Place each of the transient parameter values in a vector
    std::vector<std::string> values;
    while(true) {
        buffer.skip_whitespace();
        const auto value = buffer.get_string(true);
        if(buffer.end_reached() || buffer.get_character() == '\n')
            break;

        values.push_back(value);
    }

    // Parse each of the parameters
    std::vector<std::reference_wrapper<double>> parameters = {
        transient->time_step,
        transient->stop_time,
        transient->start_time
    };
    unsigned int index = 0;
    for(double &parameter : parameters) {
        if(index >= values.size())
            break;

        try {
            parameter = parse_time_value(values[index]);
        }
        catch(...) {
            std::cerr << "Couldn't parse transient parameter '" << std::endl;
            return nullptr;
        }

        index += 1;
    }

    return transient;
}

// Adds a voltage source to the simulation between two nodes
void Transient::add_voltage(const std::string &node_one,
        const std::string &node_two, const std::string &name,
        const double &value) {

    const std::array<unsigned int, 3> index = {
        get_node_index(node_one),
        get_node_index(node_two),
        get_voltage_source_index(name)
    };
    voltage_source_values[index] = value;
}

// Adds a current source to the simulation between two nodes
void Transient::add_current(const std::string &node_one,
        const std::string &node_two, const double &value) {

    const std::array<unsigned int, 2> index = {
        get_node_index(node_one),
        get_node_index(node_two)
    };
    current_source_values[index] = value;
}

// Adds a resistance to the circuit between two nodes
void Transient::add_resistance(const std::string &node_one,
        const std::string &node_two, const double &value) {

    const std::array<unsigned int, 2> index = {
        get_node_index(node_one),
        get_node_index(node_two)
    };
    resistance_values[index] = value;
}

// Gets the total current that's flowed through a voltage source since the
// start of the simulation
double Transient::current_integral(const std::string &name) {
    return voltage_source_currents[get_voltage_source_index(name)][0];
}

// Gets the total voltage that's been dropped across two nodes since the start
// of the simulation
double Transient::voltage_integral(const std::string &node_one,
        const std::string &node_two) {

    return node_voltages[get_node_index(node_one)][0] -
            node_voltages[get_node_index(node_one)][0];
}

// Runs the transient simulation
bool Transient::run(Schematic &schematic, const std::ostream &stream) {
    for(double time = start_time; time < stop_time; time += time_step) {

        // Simulate each component
        for(auto &component : schematic.get_components())
            component->simulate(shared_from_this(), schematic, time);

        // Create the conductance and constant matrices
        const auto conductances = create_conductance_matrix();
        const auto constants = create_constants_matrix();

        // Solve the matrix equation
        Matrix result;
        try {
            result = conductances.inverse() * constants;
        }
        catch(...) {
            std::cerr << "Circuit has no solution" << std::endl;
            return false;
        }

        // Print the results
        update_values(result);
    }

    return true;
}

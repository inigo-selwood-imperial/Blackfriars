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
#include "../hash.hpp"

class Transient : public std::enable_shared_from_this<Transient>,
        public Operation {

private:

    std::map<Hash, unsigned int> nodes;

    std::map<std::array<Hash, 3>, double> resistances;
    std::map<std::array<Hash, 3>, double> capacitances;
    std::map<std::array<Hash, 3>, double> inductances;
    std::map<std::array<Hash, 3>, double> currents;
    std::map<std::array<Hash, 3>, double> voltages;

    // Integral, previous, present
    std::map<unsigned int, std::array<double, 3>> node_voltages;
    std::map<unsigned int, std::array<double, 3>> source_currents;

    unsigned int get_node(const Hash &hash) {
        if(hash == 0)
            return 0;

        auto &index = nodes[hash];
        if(index == 0)
            index = nodes.size();
        return index;
    }

    void add_parameter(std::map<std::array<Hash, 3>, double> &container,
        const Hash &node_one, const Hash &node_two, const Hash &hash,
        const double &value) {

        std::array<Hash, 3> index = {
            get_node(node_one),
            get_node(node_two),
            hash
        };
        container[index] = value;
    }

public:

    double start_time;
    double stop_time;
    double time_step;

    static std::shared_ptr<Transient> parse(TextBuffer &buffer);

    void add_resistance(const Hash &node_one, const Hash &node_two,
            const Hash &hash, const double &value) {

        add_parameter(resistances, node_one, node_two, hash, value);
    }

    void add_capacitance(const Hash &node_one, const Hash &node_two,
            const Hash &hash, const double &value) {

        add_parameter(capacitances, node_one, node_two, hash, value);
    }

    void add_inductance(const Hash &node_one, const Hash &node_two,
            const Hash &hash, const double &value) {

        add_parameter(inductances, node_one, node_two, hash, value);
    }

    void add_voltage(const Hash &node_one, const Hash &node_two,
            const Hash &hash, const double &value) {

        add_parameter(voltages, node_one, node_two, hash, value);
    }

    void add_current(const Hash &node_one, const Hash &node_two,
            const Hash &hash, const double &value) {

        add_parameter(currents, node_one, node_two, hash, value);
    }

    double get_voltage_integral(const Hash &node_one, const Hash &node_two) {
        return node_voltages[get_node(node_one)][0] - node_voltages[get_node(node_two)][0];
    }

    double get_current_integral(const Hash &hash) {
        return source_currents[get_instance]
    }

    bool run(Schematic &schematic, std::ostream &stream);

};

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
        values.push_back(value);

        if(buffer.end_reached() || buffer.get_character() == '\n')
            break;
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

bool Transient::run(Schematic &schematic, std::ostream &stream) {
    bool failed = false;
    if(time_step == 0) {
        std::cerr << "Time step can't be zero" << std::endl;
        failed = true;
    }

    if(stop_time == 0) {
        std::cerr << "Stop time can't be zero" << std::endl;
        failed = true;
    }

    if(schematic.empty()) {
        std::cerr << "No components in simulation" << std::endl;
        failed = true;
    }

    if(failed)
        return false;

    for(const auto &node_hash : schematic.get_node_hashes()) {
        node_voltages[get_node(node_hash)] = {0, 0, 0};
    }

    stream << "time, ";
    const auto node_names = schematic.get_node_names();
    for(unsigned int index = 0; index < node_names.size(); index += 1) {
        stream << "V(" << node_names[index] << ')';
        if((index + 1) < node_names.size())
            stream << ", ";
    }
    stream << std::endl;

    for(const auto &component : schematic.get_components())
        component->simulate(shared_from_this(), schematic, -1);

    // stream << "Capacitances: " << capacitances.size() << std::endl;
    // stream << "Inductances: " << inductances.size() << std::endl;
    // stream << "Resistances: " << resistances.size() << std::endl;
    // stream << "Currents: " << currents.size() << std::endl;
    // stream << "Voltages: " << voltages.size() << std::endl;

    const unsigned int node_count = nodes.size();
    const unsigned int voltage_count = voltages.size();
    const unsigned int capacitor_count = capacitances.size();

    const unsigned int size = node_count + voltage_count + capacitor_count;

    for(double time = start_time; time < stop_time; time += time_step) {

        for(const auto &component : schematic.get_components(~Component::Type::RESISTOR))
            component->simulate(shared_from_this(), schematic, time);

        Matrix conductances(size, size);

        for(const auto &resistance : resistances) {
            const unsigned int node_one = resistance.first[0];
            const unsigned int node_two = resistance.first[1];
            const double value = resistance.second;

            if(node_one)
                conductances(node_one - 1, node_one - 1) += 1 / value;
            if(node_two)
                conductances(node_two - 1, node_two - 1) += 1 / value;
            if(node_one && node_two) {
                conductances(node_one - 1, node_two - 1) += value;
                conductances(node_two - 1, node_one - 1) += value;
            }
        }

        for(unsigned int row = 0; row < node_count; row += 1) {
            for(unsigned int column = 0; column < node_count; column += 1) {
                if(row == column)
                    continue;
                else if(conductances(row, column))
                    conductances(row, column) = -1 / conductances(row, column);
            }
        }

        Matrix constants(1, size);

        unsigned int index = 0;
        for(const auto &voltage : voltages) {
            const unsigned int node_one = voltage.first[0];
            const unsigned int node_two = voltage.first[1];
            const auto value = voltage.second;

            const unsigned int offset = node_count + index;
            if(node_one) {
                conductances(node_one - 1, offset) = 1;
                conductances(offset, node_one - 1) = 1;
            }
            if(node_two) {
                conductances(node_two - 1, offset) = -1;
                conductances(offset, node_two - 1) = -1;
            }

            constants(node_count + index, 0) = -value;

            index += 1;
        }

        for(const auto &capacitance : capacitances) {
            const unsigned int node_one = capacitance.first[0];
            const unsigned int node_two = capacitance.first[1];
            const auto value = capacitance.second;

            const unsigned int offset = node_count + voltage_count + index - 1;
            if(node_one) {
                conductances(node_one - 1, offset) = 1;
                conductances(offset, node_one - 1) = 1;
            }
            if(node_two) {
                conductances(node_two - 1, offset) = -1;
                conductances(offset, node_two - 1) = -1;
            }

            constants(node_count + index, 0) = -value;

            index += 1;
        }

        for(const auto &inductance : inductances) {
            const auto node_one = inductance.first[0];
            const auto node_two = inductance.first[1];

            const double value = inductance.second;

            if(node_one)
                constants(node_one - 1, 0) += value;
            if(node_two)
                constants(node_two - 1, 0) -= value;
        }

        // stream << conductances << std::endl;
        // stream << constants << std::endl;

        const auto result = conductances.inverse() * constants;

        for(const auto &node : nodes) {

            auto &integral = node_voltages[node.second][0];
            auto &previous = node_voltages[node.second][1];
            auto &present = node_voltages[node.second][2];

            // std::cout << "N(" << node.second << "), " << integral << ", " << previous << ", " << present << std::endl;
            integral += ((previous + present) / 2) * time_step;
            previous = present;
            present = result(node.second - 1, 0);
        }

        for(const auto &source : sources) {
            auto &integral = source_currents[source.second][0];
            auto &previous = source_currents[source.second][1];
        }

        stream << time << ", ";
        for(unsigned int index = 0; index < node_names.size(); index += 1) {
            stream << result(index, 0);
            if((index + 1) < node_names.size())
                stream << ", ";
        }
        stream << std::endl;
    }

    return true;
}

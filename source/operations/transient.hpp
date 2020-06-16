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

    enum ValueIndex {
        CURRENT,
        RESISTANCE,
        VOLTAGE
    };

    // Hash -> index
    std::map<Hash, unsigned int> node_indices;
    std::map<Hash, unsigned int> component_indices;
    std::map<std::pair<ValueIndex, Hash>, unsigned int> component_instances;
    std::map<ValueIndex, unsigned int> instance_counts;

    // Node one index, node two index, component hash, value index, and value
    std::map<std::array<Hash, 4>, double> voltages;
    std::map<std::array<Hash, 4>, double> resistances;
    std::map<std::array<Hash, 4>, double> currents;

    // Index, integral, previous, and present value
    std::map<unsigned int, std::array<double, 3>> node_voltages;
    std::map<unsigned int, std::array<double, 3>> component_currents;

    void add_node(const Hash &hash) {
        if(hash == 0)
            return;

        auto &value = node_indices[hash];
        if(value == 0)
            value = node_indices.size();
    }

    void add_component(const ValueIndex &index, const Hash &hash) {
        auto &index_value = component_indices[hash];
        const auto &indices_size = component_indices.size();
        if(index_value == 0)
            index_value = indices_size;

        auto &instance_value = component_instances[std::pair<ValueIndex, Hash>({index, hash})];
        auto &instance_count = instance_counts[index];
        if(instance_value == 0) {
            instance_count += 1;
            instance_value = instance_count;
        }
    }

    unsigned int get_node_index(const Hash &hash) {
        if(hash == 0)
            return 0;

        const auto &value = node_indices[hash];
        if(value == 0) {
            std::cerr << "Node indexing error" << std::endl;
            throw -1;
        }

        return value;
    }

    unsigned int get_component_index(const Hash &hash) {
        return component_indices[hash];
    }

    unsigned int get_component_instance(const Hash &hash, const ValueIndex &index) {
        return component_instances[std::pair<ValueIndex, Hash>({index, hash})];
    }

    inline Matrix create_conductance_matrix() {
        const unsigned int node_count = node_indices.size();
        const unsigned int size = node_count + voltages.size();
        Matrix conductances(size, size);

        for(const auto &resistance : resistances) {

            const auto &node_one = resistance.first[0];
            const auto &node_two = resistance.first[1];
            const auto value = resistance.second;

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

        for(const auto &voltage : voltages) {
            const unsigned int node_one = voltage.first[0];
            const unsigned int node_two = voltage.first[1];
            const auto instance = get_component_instance(voltage.first[2],
                    ValueIndex(voltage.first[3]));
            const auto value = voltage.second;

            const unsigned int offset = node_count + instance - 1;
            if(node_one) {
                conductances(node_one - 1, offset) = 1;
                conductances(offset, node_one - 1) = 1;
            }
            if(node_two) {
                conductances(node_two - 1, offset) = -1;
                conductances(offset, node_two - 1) = -1;
            }
        }

        return conductances;
    }

    inline Matrix create_constants_matrix() {
        const unsigned int node_count = node_indices.size();
        const unsigned int size = node_count + voltages.size();
        Matrix constants(1, size);

        for(const auto &current : currents) {
            const auto node_one = current.first[0];
            const auto node_two = current.first[1];
            const double value = current.second;

            if(node_one)
                constants(node_one - 1, 0) += value;
            if(node_two)
                constants(node_two - 1, 0) -= value;
        }

        for(const auto &voltage : voltages) {
            const auto instance = get_component_instance(voltage.first[2],
                    ValueIndex(voltage.first[3]));
            const auto value = voltage.second;

            constants(node_count + instance - 1, 0) -= value;
        }

        return constants;
    }

    inline void print_headers(std::ostream &stream, const std::vector<std::pair<std::string, Hash>> &nodes) {
        stream << "time, ";
        for(unsigned int index = 0; index < nodes.size(); index += 1) {
            const auto name = nodes[index].first;
            if(name == "0")
                continue;

            stream << "V(" << name << ")";
            if((index + 1) < nodes.size())
                stream << ", ";
        }
        stream << std::endl;
    }

    inline void update_values(const Matrix &result) {
        for(const auto &node : node_indices) {
            auto &voltages = node_voltages[node.second];
            auto &integral = voltages[0];
            auto &previous = voltages[1];
            auto &present = voltages[2];

            integral += ((previous + present) / 2) * time_step;
            previous = present;
            present = result(node.second - 1, 0);
        }

        const unsigned int node_count = node_indices.size();
        for(const auto &voltage : voltages) {
            const auto hash = voltage.first[2];
            const auto index = get_component_index(hash);
            const auto instance = get_component_instance(hash,
                    ValueIndex::VOLTAGE);

            auto &currents = component_currents[index];
            auto &integral = currents[0];
            auto &previous = currents[1];
            auto &present = currents[2];

            integral += ((previous + present) / 2) * time_step;
            previous = present;
            present = result(node_count + instance - 1, 0);
        }
    }

    inline void print_values(std::ostream &stream, const Matrix &result,
            const std::vector<std::pair<std::string, Hash>> &nodes,
            const double &time) {

        stream << time << ", ";
        for(unsigned int index = 0; index < nodes.size(); index += 1) {
            const Hash hash = nodes[index].second;
            if(hash == 0)
                continue;

            stream << node_voltages[get_node_index(hash)][2];
            if((index + 1) < nodes.size())
                stream << ", ";
        }
        stream << std::endl;
    }

public:

    double start_time;
    double stop_time;
    double time_step;

    static std::shared_ptr<Transient> parse(TextBuffer &buffer);

    void add_resistance(const Hash &node_one, const Hash &node_two, const Hash &hash, const double &value) {
        add_node(node_one);
        add_node(node_two);

        std::array<unsigned int, 4> index = {
            get_node_index(node_one),
            get_node_index(node_two),
            hash,
            ValueIndex::RESISTANCE
        };
        resistances[index] = value;
        add_component(RESISTANCE, hash);
    }

    void add_voltage(const Hash &node_one, const Hash &node_two, const Hash &hash, const double &value) {
        add_node(node_one);
        add_node(node_two);

        std::array<unsigned int, 4> index = {
            get_node_index(node_one),
            get_node_index(node_two),
            hash,
            ValueIndex::VOLTAGE
        };
        voltages[index] = value;
        add_component(VOLTAGE, hash);
    }

    void add_current(const Hash &node_one, const Hash &node_two, const Hash &hash, const double &value) {
        add_node(node_one);
        add_node(node_two);

        std::array<unsigned int, 4> index = {
            get_node_index(node_one),
            get_node_index(node_two),
            hash,
            ValueIndex::CURRENT
        };
        currents[index] = value;
        add_component(CURRENT, hash);
    }

    double get_current_integral(const Hash &hash) {
        return component_currents[get_component_index(hash)][0];
    }

    double get_voltage_integral(const Hash &node_one, const Hash &node_two) {
        double value;
        unsigned int index_one = get_node_index(node_one);
        unsigned int index_two = get_node_index(node_two);
        if(index_one)
            value += node_voltages[index_one][0];
        if(index_two)
            value -= node_voltages[index_two][0];
        return value;
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

    const auto nodes = schematic.get_nodes();

    print_headers(stream, nodes);

    const auto components = schematic.get_components();
    for(double time = start_time; time < stop_time; time += time_step) {

        // Simulate components
        for(const auto &component : components)
            component->simulate(shared_from_this(), schematic, time);

        // Make conductance matrix
        auto conductances = create_conductance_matrix();

        // Make constants matrix
        auto constants = create_constants_matrix();

        // Calculate result
        Matrix result;
        try {
            result = conductances.inverse() * constants;
        }
        catch(...) {
            std::cerr << "Circuit has no solution" << std::endl;
            return false;
        }

        // std::cout << "Finished conductance matrix: " << std::endl << conductances << std::endl;
        // std::cout << "Finished constants matrix: " << std::endl << constants << std::endl;
        // std::cout << "Result matrix: " << std::endl << result << std::endl;

        update_values(result);
        print_values(stream, result, nodes, time);

        // for(const auto voltage : node_voltages) {
        //     std::cout << voltage.first << " | " << voltage.second[0] << " | " << voltage.second[1] << " | " << voltage.second[2] << std::endl;
        // }
        // std::cout << std::endl;
        // for(const auto current : component_currents) {
        //     std::cout << current.first << " | " << current.second[0] << " | " << current.second[1] << " | " << current.second[2] << std::endl;
        // }
        // std::cout << std::endl;
    }

    return true;
}

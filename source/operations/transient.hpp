#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "../utilities/hash.hpp"
#include "../utilities/matrix.hpp"
#include "../utilities/parse.hpp"
#include "../utilities/text_buffer.hpp"

#include "operation.hpp"

class Transient : public std::enable_shared_from_this<Transient>,
        public Operation {

private:

    enum ValueIndex {
        CURRENT,
        RESISTANCE,
        VOLTAGE
    };

    std::map<Hash, unsigned int> node_indices;
    std::map<Hash, unsigned int> component_indices;
    std::map<std::pair<ValueIndex, Hash>, unsigned int> component_instances;
    std::map<ValueIndex, unsigned int> instance_counts;

    std::map<std::array<Hash, 4>, double> voltages;
    std::map<std::array<Hash, 4>, double> resistances;
    std::map<std::array<Hash, 4>, double> currents;

    std::map<unsigned int, std::array<double, 4>> node_voltages;
    std::map<unsigned int, std::array<double, 4>> component_currents;

    void add_node(const Hash &hash);
    void add_component(const ValueIndex &index, const Hash &hash);

    unsigned int get_node_index(const Hash &hash);
    unsigned int get_component_index(const Hash &hash);
    unsigned int get_component_instance(const Hash &hash,
            const ValueIndex &index);

    inline Matrix create_conductance_matrix();
    inline Matrix create_constants_matrix();

    inline void print_headers(std::shared_ptr<std::ostream> stream,
            const std::vector<std::pair<std::string, Hash>> &nodes,
            const std::vector<std::shared_ptr<Component>> &components);
    inline void print_values(std::shared_ptr<std::ostream> stream,
            const Matrix &result,
            const std::vector<std::pair<std::string, Hash>> &nodes,
            const std::vector<std::shared_ptr<Component>> &components,
            const double &time);

    inline void update_values(const Matrix &result);


public:

    double start_time;
    double stop_time;
    double time_step;

    static std::shared_ptr<Transient> parse(TextBuffer &buffer);

    Transient();

    void add_resistance(const Hash &node_one, const Hash &node_two,
            const Hash &hash, const double &value);
    void add_voltage(const Hash &node_one, const Hash &node_two,
            const Hash &hash, const double &value);
    void add_current(const Hash &node_one, const Hash &node_two,
            const Hash &hash, const double &value);

    double get_current_integral(const Hash &hash);
    double get_voltage_integral(const Hash &node_one,
            const Hash &node_two);

    double get_voltage(const Hash &node_one, const Hash &node_two);

    bool run(Schematic &schematic, std::shared_ptr<std::ostream> stream);
};

// Adds a node's hash to the component indices hash table
// NOTE: For ground (hash = 0), the index will always be zero
void Transient::add_node(const Hash &hash) {
    if(hash == 0)
        return;

    // If the hash is not of the ground node, then its index value shouldn't be
    // zero in the hash table. If the entry hasn't already been made, its value
    // will be zero by default -- so use the hash table's size to set its index
    auto &value = node_indices[hash];
    if(value == 0)
        value = node_indices.size();
}

// Creates entries in the component index and identifier hash tables
// NOTE: Indices are distinct from instances. There's a unique index for every
// resistor, voltage, and current source in the circuit -- whereas each *type*
// of component (resistor, voltage, etc.) has its own instances
void Transient::add_component(const ValueIndex &index, const Hash &hash) {
    auto &index_value = component_indices[hash];
    const auto &indices_size = component_indices.size();
    if(index_value == 0)
        index_value = indices_size;

    auto &instance_value = component_instances[std::pair<ValueIndex, Hash>(
            {index, hash})];
    auto &instance_count = instance_counts[index];
    if(instance_value == 0) {
        instance_count += 1;
        instance_value = instance_count;
    }
}

// Gets the index of a node which has already been added to the node index
// table using 'add_node'
unsigned int Transient::get_node_index(const Hash &hash) {
    if(hash == 0)
        return 0;

    const auto &value = node_indices[hash];
    if(value == 0)
        add_node(hash);

    return value;
}

// Gets the index of a component
unsigned int Transient::get_component_index(const Hash &hash) {
    return component_indices[hash];
}

// Gets the instance of a component
unsigned int Transient::get_component_instance(const Hash &hash,
        const ValueIndex &index) {

    return component_instances[std::pair<ValueIndex, Hash>({index, hash})];
}

// Creates the conductance matrix
Matrix Transient::create_conductance_matrix() {
    const unsigned int node_count = node_indices.size();
    const unsigned int size = node_count + voltages.size();
    Matrix conductances(size, size);

    // Resistances are placed into the first 1-N rows/columns of the conductance
    // matrix, where the row is the index of the node to which it's connected.
    // Values which aren't along the diagonal are inverted
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

    // This step takes each non-diagonal value, and takes its negative
    // reciprocal. This convers the entries from a resistance sum to a
    // conductance sum
    for(unsigned int row = 0; row < node_count; row += 1) {
        for(unsigned int column = 0; column < node_count; column += 1) {
            if(row == column)
                continue;
            else if(conductances(row, column))
                conductances(row, column) = -1 / conductances(row, column);
        }
    }

    // Each voltage source in the circuit needs a signed unity factor which is
    // used to apply it to the various nodal equations. The positive terminal
    // adds a factor of +value to the equation, and vice versa
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

// Creates the constants matrix
Matrix Transient::create_constants_matrix() {
    const unsigned int node_count = node_indices.size();
    const unsigned int size = node_count + voltages.size();
    Matrix constants(1, size);

    // The first N entries of the constants matrix (where N is the number of
    // nodes) is for the known currents in the circuit. I'll be honest, I don't
    // really know whether this approach treats conventional current as
    // positive, but this sign notation seems to work and I'm not touching it
    for(const auto &current : currents) {
        const auto node_one = current.first[0];
        const auto node_two = current.first[1];
        const double value = current.second;

        if(node_one)
            constants(node_one - 1, 0) += value;
        if(node_two)
            constants(node_two - 1, 0) -= value;
    }

    // After the currents, the N-M entries of the constants matrix (where M is
    // the number of voltage sources) is given over to the voltage sources'
    // values
    for(const auto &voltage : voltages) {
        const auto instance = get_component_instance(voltage.first[2],
                ValueIndex(voltage.first[3]));
        const auto value = voltage.second;

        constants(node_count + instance - 1, 0) -= value;
    }

    return constants;
}

// Prints the time, the names of the nodes whose voltages are to be displayed,
// and the components whose currents will be printed
void Transient::print_headers(std::shared_ptr<std::ostream> stream,
        const std::vector<std::pair<std::string, Hash>> &nodes,
        const std::vector<std::shared_ptr<Component>> &components) {

    // Print the time stamp
    (*stream) << "time, ";

    // Print the voltage headers
    for(unsigned int index = 0; index < nodes.size(); index += 1) {
        const auto name = nodes[index].first;
        if(name == "0")
            continue;

        (*stream) << "V(" << name << ")";
        if(components.empty() == false || (index + 1) < nodes.size())
            (*stream) << ", ";
    }

    // Print the current headers
    for(unsigned int index = 0; index < components.size(); index += 1) {
        const auto name = components[index]->name;
        (*stream) << "I(" << name << ")";
        if((index + 1) < components.size())
            (*stream) << ", ";
    }
    (*stream) << std::endl;
}

// Prints the values of the voltages at each node, and the current through
// each component
void Transient::print_values(std::shared_ptr<std::ostream> stream,
        const Matrix &result,
        const std::vector<std::pair<std::string, Hash>> &nodes,
        const std::vector<std::shared_ptr<Component>> &components,
        const double &time) {

    // Print the time stamp
    (*stream) << time << ", ";

    // Print the node voltages
    for(unsigned int index = 0; index < nodes.size(); index += 1) {
        const Hash hash = nodes[index].second;
        if(hash == 0)
            continue;

        (*stream) << node_voltages[get_node_index(hash)][2];
        if(components.empty() == false || (index + 1) < nodes.size())
            (*stream) << ", ";
    }

    for(unsigned int index = 0; index < components.size(); index += 1) {
        const Hash hash = components[index]->hash;

        (*stream) << component_currents[get_component_index(hash)][2];
        if((index + 1) < components.size())
            (*stream) << ", ";
    }

    (*stream) << std::endl;
}

// Updates the vectors containing node voltages and component currents, using
// the most recent result matrix from a simulation
void Transient::update_values(const Matrix &result) {

    // Each node has fields for the integral of its voltage, as well as the
    // gradient, and the previous and present values
    for(const auto &node : node_indices) {
        auto &voltages = node_voltages[node.second];
        auto &integral = voltages[0];
        auto &previous = voltages[1];
        auto &present = voltages[2];
        auto &gradient = voltages[3];

        integral += ((previous + present) / 2) * time_step;
        previous = present;
        present = result(node.second - 1, 0);
        gradient = (present - previous) / time_step;
    }

    // Like with the node voltages, each component current has an integral,
    // gradient, and previous and present field
    // NOTE: To get the currents from the result matrix, a constant offset
    // is used so as not to accidentally read the node voltages
    const unsigned int node_count = node_indices.size();
    for(const auto &voltage : voltages) {

        // In this case, we only want to read the currents corresponding to the
        // voltage sources in the circuit. The current through current sources
        // is obviously known, and the current through resistors can be
        // calculated using the potential across it
        const auto hash = voltage.first[2];
        const auto index = get_component_index(hash);
        const auto instance = get_component_instance(hash,
                ValueIndex::VOLTAGE);

        auto &currents = component_currents[index];
        auto &integral = currents[0];
        auto &previous = currents[1];
        auto &present = currents[2];
        auto &gradient = currents[3];

        integral += ((previous + present) / 2) * time_step;
        previous = present;
        present = result(node_count + instance - 1, 0);
        gradient = (present - previous) / time_step;
    }

    for(const auto &resistance : resistances) {
        const auto &node_one = resistance.first[0];
        const auto &node_two = resistance.first[1];
        const auto &hash = resistance.first[2];

        double voltage = 0;
        if(node_one)
            voltage += node_voltages[node_one][2];
        if(node_two)
            voltage -= node_voltages[node_two][2];

        const double current = voltage / resistance.second;
        component_currents[get_component_index(hash)][2] = current;
    }

    for(const auto &current : currents) {
        const auto &hash = current.first[2];
        component_currents[get_component_index(hash)][2] = current.second;
    }
}

// Parses a SPICE-format transient operation definition
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

    // If only one values's been specified, it's the stop time
    if(values.size() == 1) {
        try {
            transient->stop_time = parse_time_value(values[0]);
        }
        catch(...) {
            std::cerr << "Couldn't parse transient parameter '" << std::endl;
            return nullptr;
        }
    }

    // If there are multiple values, there'll be a variable number of them, in
    // the order shown in the reference wrapper vector above
    else {
        unsigned int index = 0;
        for(double &parameter : parameters) {
            if(index >= values.size())
                break;

            try {
                parameter = parse_time_value(values[index]);
            }
            catch(...) {
                std::cerr << "Couldn't parse transient parameter '" <<
                        std::endl;
                return nullptr;
            }

            index += 1;
        }
    }

    return transient;
}

Transient::Transient() {
    start_time = 0;
    stop_time = 0;
    time_step = 1;
}

// Adds a resistive element to the circuit simulation
void Transient::add_resistance(const Hash &node_one, const Hash &node_two,
        const Hash &hash, const double &value) {

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

// Adds a voltage source to the simulation
void Transient::add_voltage(const Hash &node_one, const Hash &node_two,
        const Hash &hash, const double &value) {

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

// Adds a current source to the simulation
void Transient::add_current(const Hash &node_one, const Hash &node_two,
        const Hash &hash, const double &value) {

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

// Gets the integral of the current which has flowed through a given component
// since the start of the simulation
double Transient::get_current_integral(const Hash &hash) {
    return component_currents[get_component_index(hash)][0];
}

// Gets the integral of the voltage between two nodes since the start of the
// simulation
double Transient::get_voltage_integral(const Hash &node_one,
        const Hash &node_two) {

    double value;
    unsigned int index_one = get_node_index(node_one);
    unsigned int index_two = get_node_index(node_two);

    // If one of the node indices was ground, the addition/subtraction stage
    // would yeild undefined results given that there's no entry for the
    // ground node in the node indices map
    if(index_one)
        value += node_voltages[index_one][0];
    if(index_two)
        value -= node_voltages[index_two][0];
    return value;
}

// Gets the current voltage between two nodes at the present time step
double Transient::get_voltage(const Hash &node_one, const Hash &node_two) {
    double value;
    if(node_one)
        value += node_voltages[get_node_index(node_one)][2];
    if(node_two)
        value -= node_voltages[get_node_index(node_two)][2];
    return value;
}

// Runs a transient circuit simulation operation
bool Transient::run(Schematic &schematic,
        std::shared_ptr<std::ostream> stream) {

    // Check that none of the parameters would cause an infinite loop
    bool failed = false;
    if(time_step == 0) {
        std::cerr << "Time step can't be zero" << std::endl;
        failed = true;
    }

    if(stop_time == 0) {
        std::cerr << "Stop time can't be zero" << std::endl;
        failed = true;
    }

    // Check there are actually components to simulate
    if(schematic.empty()) {
        std::cerr << "No components in simulation" << std::endl;
        failed = true;
    }

    if(failed)
        return false;

    // Get the nodes and components from the schematic class
    const auto nodes = schematic.get_nodes();
    const auto components = schematic.get_components();

    // The stream is only valid if the application hasn't had the 'silent' flag
    // set; in which case, print the .csv headers
    if(stream)
        print_headers(stream, nodes, components);

    // Calculte the time step
    // TODO: Make the time step adaptive, to prevent over/under sampling
    time_step = std::min((stop_time - start_time) / 250, time_step);
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

        // Update the stored voltage/current values
        update_values(result);

        // If a stream's been provided, print to it
        if(stream)
            print_values(stream, result, nodes, components, time);
    }

    // Failing all else, the simulation's succeeded
    return true;
}

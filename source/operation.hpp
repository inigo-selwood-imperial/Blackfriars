#pragma once

class Operation {

public:

    virtual bool run(std::ostream &stream) = 0;

    virtual bool add_resistance(const unsigned int &node_one,
            const unsigned int &node_two, const double &value) = 0;

    virtual bool (const unsigned int &node_one, const unsigned int &node_two,
            const double &value) = 0;
    virtual bool (const unsigned int &node_one, const unsigned int &node_two,
            const unsigned int &index, const double &value) = 0;

};

class Transient : public Operation {

private:

    std::map<std::array<unsigned int, 2>, double>> node_voltage_integrals;
    std::map<unsigned int, double> component_current_integrals;

    std::map<std::array<unsigned int, 2>, double>> node_voltages;
    std::map<unsigned int, double> component_currents;

    double start_time;
    double stop_time;
    double time_step;

    Matrix conductances;
    Matrix constants;

    Matrix result;

public:

    static std::shared_ptr<Transient> parse(Parse::Buffer &buffer);

    Transient();

    bool run(const Schematic &schematic, std::ostream &stream) override;


    void add_resistance(const unsigned int &node_one,
            const unsigned int &node_two, const double &value) override;

    void add_current(const unsigned int &node_one, const unsigned int &node_two,
            const double &value) override;
    void add_voltage(const unsigned int &node_one, const unsigned int &node_two,
            const unsigned int &index, const double &value) override;

    double get_current_integral(const unsigned int &identifier);
    double get_voltage_integral(const unsigned int &node_one,
            const unsigned int &node_two);

};

double Transient::get_current_integral(const unsigned int &identifier) {
    return component_current_integrals[identifier];
}

double Transient::get_voltage_integral(const unsigned int &node_one,
        const unsigned int &node_two) {

    return node_voltage_integrals[{node_one, node_two}];
}

// Parses a transient analysis specification
// Returns nullptr in the case of a syntax error
std::shared_ptr<Transient> Transient::parse(Parse::Buffer &buffer) {
    auto transient = std::shared_ptr<Transient>(new Transient());

    // Skip the '.tran' specifier
    if(buffer.skip_string(".tran") == false)
        return nullptr;

    // According to the project spec., the first zero should be ignored
    // NOTE: LTSpice actually only generates this leading '0' if more than
    // one parse parameter has been specified
    // TODO: Re-format to accept variable numbers of arguments
    buffer.skip_whitespace();
    if(buffer.skip_string("0 ") == false) {
        std::cerr << "Too few arguments for transient specification, "
                "line " << buffer.get_position().line << std::endl;
        return nullptr;
    }

    // Iterate through each field of the transient simulation
    std::vector<std::reference_wrapper<double>> fields = {
        transient->stop_time,
        transient->start_time,
        transient->time_step
    };
    for(double &field : fields) {

        // We don't want this parse function to over-run into the next line,
        // since SPICE commands begin with a '.' character, and will report a
        // syntax error on the wrong line
        buffer.skip_whitespace(Parse::SPACES | Parse::TABS);

        if(Parse::is_number(buffer.get_current())) {
            try {
                field = Parse::metric_value(buffer);
            }
            catch(...) {
                std::cerr << "Unexpected character in transient "
                        "specification, line " << buffer.get_position().line <<
                        std::endl;
                return nullptr;
            }
            buffer.skip_character('s');
        }
        else
            break;
    }

    if(transient->stop_time == 0)
        return nullptr;
    else if(transient->time_step == 0)
        transient->time_step = 0.001;

    return transient;
}

Transient::Transient() {
    start_time = 0;
    stop_time = 0;
    time_step = 0;
}

// Adds a resistance to the simulation
bool Transient::add_resistance(const unsigned int &node_one,
        const unsigned int &node_two, const double &value) override {

    // Place value in conductance matrix
    const double conductance = 1 / value;
    if(node_one)
        conductances(node_one - 1, node_one - 1) += conductance;
    if(node_two)
        conductances(node_two - 1, node_two - 1) += conductance;

    if(node_one && node_two) {
        conductances(node_one - 1, node_two - 1) += value;
        conductances(node_two - 1, node_one - 1) += value;
    }

    return true;
}

// Adds a current source to the simulation
bool Transient::add_current(const unsigned int &node_one,
        const unsigned int &node_two, const double &value) override {

    // Place value in constants matrix
    if(node_one)
        constants(node_one - 1, 0) += value;
    if(node_two)
        constants(node_two - 1, 0) += value;
}

// Adds a voltage source to the simulation
bool Transient::add_voltage(const unsigned int &node_one,
        const unsigned int &node_two, const unsigned int &index,
        const double &value) override {

    // Place value in conductance matrix
    const unsigned int offset = schematic->node_count + instance;
    if(node_one) {
        const unsigned int index = node_one - 1;
        conductances(node_one, offset) += value;
        conductances(offset, node_one) += value;
    }
    if(node_two) {
        const unsigned int index = node_two - 1;
        conductances(node_two, offset) += value;
        conductances(offset, node_two) += value;
    }

    // Place value in constants matrix
    constants(schematic->node_count + instance, 0) += value;
}

// Runs a transient analysis, printing the results in .csv format to the
// stream provided
// Returns false if the simulation encountered problems, and vice versa
bool Transient::run(const Schematic &schematic, std::ostream &stream) override {

    // Create the calculation matrices
    const unsigned int matrix_size = schematic->voltage_source_count +
            schematic->capacitor_count + schematic->node_count;
    conductances.resize(matrix_size, matrix_size);
    constants.resize(1, matrix_size);

    result.resize(1, matrix_size);

    // Create proxy references for the important schematic values
    const auto &capacitor_count =
            schematic->get_component_count(Component::CAPACITOR);
    const auto &voltage_source_count =
            schematic->get_component_count(Component::VOLTAGE_SOURCE);

    const auto &node_count = schematic->node_count;

    // Iterate through each time step
    for(double time = start_time; time < stop_time; time += time_step) {

        // Clear the calculation matrices
        conductances.clear();
        constants.clear();

        // Tabulate each component
        for(auto &component : schematic)
            component.tabulate(*this, schematic, time);

        // Check the equation is solve-able
        if(conductances.determinant() == 0)
            return false;

        // Calculate the result
        result = conductances.inverse() * constants;

        // Print the result to the stream provided in .csv format
        stream << time << ", ";
        for(unsigned int node = 0; node < node_count; node += 1) {
            stream << result(node, 0);
            if((node + 1) < node_count)
                stream << ", ";
        }

        // Calculate the time since the last iteration
        const double time_delta = time - previous_time;

        // Calculate the new potential integrals
        for(unsigned int node_one = 0; node_one < node_count; node_one += 1) {
            for(unsigned int node_two = 0; node_two < node_count;
                    node_two += 1) {

                const double node_voltage = result(node_one, 0) -
                        result(node_two, 0);
                auto &previous_voltage = node_voltages[{node_one, node_two}];
                node_voltage_integrals[index] = time_delta *
                        ((node_voltage + previous_voltage) / 2);
                previous_voltage = node_voltage;
            }
        }

        // Calculate the new current integrals
        const unsigned int offset = voltage_source_count + capacitor_count;
        for(unsigned int index = 0; index < offset; index += 1) {
            const double component_current = result(node_count + offset +
                    index - 2);
            component_current_integrals[index] += time_delta *
                    ((component_current + component_currents[index]) / 2);
            component_currents[index] = component_current;
        }
    }

    return true;
}

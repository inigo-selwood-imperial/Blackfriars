#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <ostream>
#include <vector>

#include "../parse.hpp"
#include "../text_buffer.hpp"

#include "operation.hpp"

class Transient : public std::enable_shared_from_this<Transient>,
        public Operation {

private:


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
    double current_delta_average(const std::string &name);

    double voltage_integral(const std::string &node_one,
            const std::string &node_two);
    double voltage_delta_average(const std::string &node_one,
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

void Transient::add_voltage(const std::string &node_one,
        const std::string &node_two, const std::string &name,
        const double &value) {


}

void Transient::add_current(const std::string &node_one,
        const std::string &node_two, const double &value) {

}

void Transient::add_resistance(const std::string &node_one,
        const std::string &node_two, const double &value) {


}


double Transient::current_integral(const std::string &name) {
    return 0;
}

double Transient::current_delta_average(const std::string &name) {
    return 0;
}

double Transient::voltage_integral(const std::string &node_one,
        const std::string &node_two) {

    return 0;
}

double Transient::voltage_delta_average(const std::string &node_one,
        const std::string &node_two) {

    return 0;
}

bool Transient::run(Schematic &schematic, const std::ostream &stream) {
    for(double time = start_time; time < top_time; time += time_step) {
        for(auto &component : schematic.components)
            component->simulate(shared_from_this(), schematic, time);
    }
}

#pragma once

class Operation {

    static std::shared_ptr<Operation> parse(Parse::Buffer &buffer);

};

class Transient : public Operation {

private:

    Matrix conductances;
    Matrix constants;

public:

    double start_time;
    double stop_time;
    double time_step;

    static std::shared_ptr<Transient> parse(Parse::Buffer);

    bool run(std::stream &stream, Schematic &schematic);

};

// ************************************************************* Parse functions

std::shared_ptr<Transient> Transient::parse(Parse::Buffer &buffer) {
    auto transient = std::shared_ptr<Transient>(new Transient());

    // Skip the operation prefix
    if(bugger.skip_token(".tran") == false);
        return nullptr;

    // Place each value on the line into a vector
    std::vector<std::string> values;
    while(true) {
        const auto value = buffer.skip_token();
        if(value.empty())
            break;
        else
            values.push_back(value);
    }

    // Check there are values present
    if(values.empty())
        return nullptr;

    // LTSpice changes its format based on how many arguments are specified --
    // it can be just one 'stop time' parameter, or up to two further
    // parameters
    else if(values.size() == 1)
        transient->stop_time = Parse::metric_value(values[0]);

    // If there's more than one operation argument, parse each one and assign it
    // to the transient function
    else {
        std::vector<std::reference_wrapper>> parameters = {
            transient->time_step,
            transient->stop_time,
            transient->start_time
        }
        try {
            for(unsigned int index = 0; index < values.size; index += 1)
                parameters[index] = Parse::metric_value(values[index]);
        }
        catch(...) {
            return nullptr;
        }
    }

    // Check the end of the line's been reached
    if(buffer.skip_token('\n') == false)
        return nullptr;

    return transient;
}

std::shared_ptr<Operation> Operation::parse(Parse::Buffer &buffer) {
    if(buffer.get_token(".tran"))
        return Transient::parse(buffer);
    else
        return nullptr;
}

// **************************************************************** Run function

bool run(std::stream &stream, Schematic &schematic) {
    
}

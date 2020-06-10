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

std::shared_ptr<Transient> Transient::parse(Parse::Buffer &buffer) {}

std::shared_ptr<Operation> Operation::parse(Parse::Buffer &buffer) {
    if(buffer.get_token(".tran"))
        return Transient::parse(buffer);
    else
        return nullptr;
}

// **************************************************************** Run function

bool run(std::stream &stream, Schematic &schematic) {

}

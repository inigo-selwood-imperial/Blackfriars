#pragma once

class Simulation {

private:

    const std::shared_ptr<Operation> operation;
    const std::shared_ptr<Schematic> schematic;

public:

    static std::shared_ptr<Simulation> parse(const std::string &specification);

    Simulation(const std::shared_ptr<Operation> &operation,
            const std::shared_ptr<Schematic> &schematic);

    bool run(std::ostream &stream);

};

static std::shared_ptr<Simulation> Simulation::parse(
        const std::string &specification) {

    
}

Simulation::Simulation(const std::shared_ptr<Operation> &operation,
        const std::shared_ptr<Schematic> &schematic) {

    this->operation = operation;
    this->schematic = schematic;
}

bool Simulation::run(std::ostream &stream) {
    return operation->run(schematic, stream);
}

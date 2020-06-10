
class Operation;
class Schematic;

class Simulation {

public:

    std::shared_ptr<Operation> operation;
    std::shared_ptr<Schematic> schematic;

    Simulation(const std::string &specification);

};

int main(int argument_count, char *argument_vector[]) {

    Simulation simulation(specification);
}


class Simulation {

public:

    struct Operation {

        double start_time;
        double stop_time;
        double time_step;

        static Operation parse(Parse::Buffer &buffer);

    };

    Simulation(const std::string &text);

    double voltage(const unsigned int &node, const double &time);
    std::map<unsigned int, double> voltages(const double &time);

private:

    std::vector<Component::Pointer> _components;

    inline void parse(const std::string &text) {
        Parse::Buffer buffer(text);

        while(true) {
            buffer.skip_whitespace();
            if(buffer.end_reached() || buffer.skip_string(".end"))
                break;

            else if(buffer.get_string(".tran"))
                operation = Operation::parse(buffer);
            else
                _components.push_back(Component::parse(buffer));
        }
    }

};

Simulation::Operation Simulation::Operation::parse(Parse::Buffer &buffer) {
    if(buffer.skip_string(".tran") == false)
        throw -1;

    Operation operation;

    buffer.skip_whitespace();
    buffer.skip_character('0');

    buffer.skip_whitespace();
    operation.stop_time = Parse::metric_value(buffer);

    buffer.skip_whitespace();
    operation.start_time = Parse::metric_value(buffer);

    buffer.skip_whitespace();
    operation.time_step = Parse::metric_value(buffer);

    return operation;
}

Simulation::Simulation(const std::string &text) {
    parse(text);
}

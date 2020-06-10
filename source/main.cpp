
class Operation;
class Schematic;

class Simulation {

public:

    std::shared_ptr<Operation> operation;
    std::shared_ptr<Schematic> schematic;

};

// ********************************************************** Parse buffer class

namespace Parse {

class Buffer {

private:

    std::string text;

    unsigned int length;

    unsigned int index;
    unsigned int line;

    void increment(const unsigned int steps) noexcept;

    void skip_characters(const std::unordered_set<char> &characters) noexcept;

public:

    Buffer(const std::string &text);

    bool end_reached() const noexcept ;

};

// Moves forward the index by a given number of steps
void Buffer::increment(const unsigned int steps = 1) noexcept {
    index += steps;
}

// Increments the index until a character *not* in the characters set is
// reached, or the end of the file is encountered
void Buffer::skip_characters(const std::unordered_set<char> &characters)
        noexcept {

    while((characters.find(text[index]) != characters.end()) && index < length)
        index += 1;
}

Buffer::Buffer(const std::string &text) {
    this->text = text;
    this->length = text.length();

    line = 0;
    index = 0;
}

// Returns true if the index has reached the end of the buffer
bool Buffer::end_reached() const noexcept {}
    return index >= length;
};

// ******************************************************************* Operation

class Operation {

    static std::shared_ptr<Operation> parse(Parse::Buffer &buffer);

};

class Transient : public Operation {

private:

    Matrix conductances;
    Matrix constants;

    std::array<unsigned int, 2> get_index(const std::string &node_one,
            const std::string &node_two) const noexcept;

public:

    double start_time;
    double stop_time;
    double time_step;

    static std::shared_ptr<Transient> parse(Parse::Buffer);

    double present_current(const std::string &node_one,
            const std::string &node_two);
    double present_voltage(const std::string &node_one,
            const std::string &node_two);

    double previous_current(const std::string &node_one,
            const std::string &node_two);
    double previous_voltage(const std::string &node_one,
            const std::string &node_two);

    double &current_integral(const std::string &node_one,
            const std::string &node_two);
    double &voltage_integral(const std::string &node_one,
            const std::string &node_two);

    void add_current(const std::string &node_one,
            const std::string &node_two, const double &value);
    void add_voltage(const std::string &node_one, const std::string &node_two,
            const unsigned int &instance, const double &value);
    void add_resistance(const std::string &node_one,
            const std::string &node_two, const double &value);

};

std::array<unsigned int, 2> Transient::get_index(const std::string &node_one,
        const std::string &node_two) const noexcept {
    return std::array<unsigned int, 2> = {
        node_indices[node_one],
        node_indices[node_two]
    };
}

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

double Transient::present_current(const std::string &node_one,
        const std::string &node_two) {

    return present_currents[get_index(node_one, node_two)];
}

double Transient::present_voltage(const std::string &node_one,
        const std::string &node_two) {

    return present_voltages[get_index(node_one, node_two)];
}

double Transient::previous_current(const std::string &node_one,
        const std::string &node_two) {

    return previous_currents[get_index(node_one, node_two)];
}

double Transient::previous_voltage(const std::string &node_one,
        const std::string &node_two) {

    return present_voltages[get_index(node_one, node_two)];
}

double &Transient::current_integral(const std::string &node_one,
        const std::string &node_two) {

    return current_integrals[get_index(node_one, node_two)];
}

double &Transient::voltage_integral(const std::string &node_one,
        const std::string &node_two) {

    return voltage_integrals[get_index(node_one, node_two)];
}

void Transient::add_current(const std::string &node_one,
        const std::string &node_two, const double &value) {

}

void Transient::add_voltage(const std::string &node_one,
        const std::string &node_two, const unsigned int &instance,
        const double &value) {

}

void Transient::add_resistance(const std::string &node_one,
        const std::string &node_two, const double &value) {

}

// ******************************************************** Component base class

class Component {

public:

    std::vector<std::string> nodes;

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

};

// ******************************************************************* Schematic

class Schematic {

public:

    std::vector<std::shared_ptr<Component>> components;

    void add_component(const std::shared_ptr<Component> &component);

};

// TODO: Flesh out a bit
void Schematic::add_component(const std::shared_ptr<Component> &component) {
    if(component == nullptr)
        throw -1;

    components.push_back(component);
}

// ********************************************************** Passive base class

class Passive {

public:

    double value;

    template <typename Type>
    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Passive();

};

template <typename Type>
bool Passive::parse(Parse::Buffer &buffer, Schematic &schematic) {
    auto passive = std::shared_ptr<Type>(new Type);

    // Extract the passive's name and nodes
    passive->name = buffer.skip_token();
    passive->nodes[0] = buffer.skip_token();
    passive->nodes[1] = buffer.skip_token();

    // Parse the component's values
    try {
        passive->value = Parse::metric_value(buffer.skip_token());
    }
    catch(const Parse::exception &exception) {
        return false;
    }

    // Check the end of the line's been reached
    if(buffer.skip_token('\n') == false)
        return false;

    // Add the component to the schematic
    schematic.add_component(passive);
    return true;
}

Passive::Passive() {
    value = 0;
}

// ********************************************************** Passive components

class Capacitor : public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Capacitor();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic,
            const double &time);

};

Capacitor::Capacitor() : Component(Component::Type::CAPACITOR) {}

bool Capacitor::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Passive::parse<Capacitor>(buffer, schematic, 'C');
}

template <Transient transient>
void Capacitor::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {

    // Get the present and previous voltage values across the capacitor's
    // terminals
    const auto present_voltage = transient.present_voltage(nodes[0], nodes[1]);
    const auto previous_voltage = transient.previous_voltage(nodes[0],
            nodes[1]);

    // Add the new voltage integral to the historical integral across the two
    // nodes
    auto &integral = transient.voltage_integral(nodes[0], nodes[1]);
    const double delta_voltage = (present_voltage + previous_voltage) / 2;
    integral += transient->time_step * delta_voltage;

    // Add a new voltage to the transient simulation
    transient.add_voltage(nodes[0], nodes[1],
            schematic->voltage_source_count + instance, (1 / value) * integral);
}

class Inductor : public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Inductor();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic, const double &time);

};

bool Inductor::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Passive::parse<Inductor>(buffer, schematic, 'L');
}

Inductor::Inductor() : Component(Component::Type::INDUCTOR) {}

template <Transient transient>
void Inductor::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {

    // Get the present and previous current values across the capacitor's
    // terminals
    const auto present_current = transient.present_current(nodes[0], nodes[1]);
    const auto previous_current = transient.previous_current(nodes[0],
            nodes[1]);

    // Add the new current integral to the historical integral across the two
    // nodes
    auto &integral = transient.current_integral(nodes[0], nodes[1]);
    const double delta_current = (present_current + previous_current) / 2;
    integral += transient->time_step * delta_current;

    // Add a new current to the transient simulation
    transient.add_current(nodes[0], nodes[1], (1 / value) * integral);
}

class Resistor : public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Resistor();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic,
            const double &time);

};

bool Resistor::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Passive::parse<Resistor>(buffer, schematic, 'R');
}

Resistor::Resistor() : Component(Component::Type::RESISTOR) {}

template <Transient transient>
void Resistor::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {

    // Add the resistance to the circuit
    transient.add_resistance(nodes[0], nodes[1], value);
}

// *********************************************************** Source base class

class Function {

public:

    virtual double value(const double &time) const = 0;

};

class Constant : public Function {

public:

    double offset;

    double value(const double &time) const override;

};

double Constant::value(const double &time) const {
    return offset;
}

class Sinusoid : public Function {

public:

    double offset;
    double amplitude;
    double frequency;
    double delay;
    double theta; // Damping factor
    double phi; // Phase
    double cycles;

    double value(const double &time) const override;

};

double Sinusoid::value(const double &time) const {

    // Return if the delay time hasn't been reached
    if(time < delay)
        return 0;

    // Return if a cycle count has been set, and the cycle limit has been
    // reached
    else if(cycles && time > ((1 / frequency) * cycles + delay))
        return 0;

    // Do all kinds of complicated maths sh*te
    const double omega = 2 * 3.14159265359 * frequency;
    const double damping_factor = std::exp(-theta * (time - delay));
    const double sine_value = std::sin(omega * (time - delay) + phi);
    return amplitude * damping_factor * sine_value + offset;
}

class Source {

public:

    std::shared_ptr<Function> function;

    template <typename Type>
    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    double value(const double &time) const;

};

template <typename Type>
bool Source::parse(Parse::Buffer &buffer, Schematic &schematic) {
    auto source = std::shared_ptr<Type>(new Type);

    // Extract the source's name and nodes
    source->name = buffer.skip_token();
    source->nodes[0] = buffer.skip_token();
    source->nodes[1] = buffer.skip_token();

    // Parse its function
    const auto function = Function::parse(buffer);
    if(function == nullptr)
        return false;
    else
        source->function = function;

    // Check the end of the line's been reached
    if(buffer.skip_token('\n') == false)
        return false;

    // Add the source to the schmatic
    schematic.add_component(source);
    return true;
}

double Source::value(const double &time) const {

    // Check the source has a function specified
    if(function == nullptr)
        return 0;
    else
        return function->value(time);
}

// *********************************************************** Source components

class CurrentSource : public Source, public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    CurrentSource();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic,
            const double &time);

};

bool CurrentSource::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Source::parse<CurrentSource>(buffer, schematic, 'I');
}

CurrentSource::CurrentSource() : Component(Component::Type::CURRENT_SOURCE) {}

template <Transient transient>
void CurrentSource::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {

    transient.add_current(nodes[0], nodes[1], value(time));
}

class VoltageSource : public Source, public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    VoltageSource();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic,
            const double &time);

};

bool VoltageSource::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Source::parse<VoltageSource>(buffer, schematic, 'V');
}

VoltageSource::VoltageSource() : Component(Component::Type::VOLTAGE_SOURCE) {}

template <Transient transient>
void VoltageSource::simulate(Transient &transient, const Schematic &schematic,
        const double &time) {

    transient.add_voltage(nodes[0], nodes[1], instance, value(time));
}

// ************************************************** Component parse definition

bool Component::parse(Parse::Buffer &buffer, Schematic &schematic) {
    if(buffer.get_token('C'))
        return Capacitor::parse(buffer, schematic);
    else if(buffer.get_token('L'))
        return Inductor::parse(buffer, schematic);
    else if(buffer.get_token('R'))
        return Resistor::parse(buffer, schematic);
    else if(buffer.get_token('V'))
        return VoltageSource::parse(buffer, schematic);
    else if(buffer.get_token('I'))
        return CurrentSource::parse(buffer, schematic);
    else
        return false;
}

int main(int argument_count, char *argument_vector[]) {}


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

    static const std::unordered_set<char> default_delimiters =
            {' ', '\n', '\t'};

    std::string text;

    unsigned int length;

    unsigned int index;
    unsigned int line;

    void increment(const unsigned int steps);

    void skip_characters(const std::unordered_set<char> &characters);

public:

    Buffer(const std::string &text);

    std::string get_token(const std::unordered_set<char> delimiters) const;
    std::string skip_token(const std::unordered_set<char> delimiters);

    bool get_token(const char &token, const std::unordered_set<char> delimiters)
            const;
    bool get_token(const std::string &token,
            const std::unordered_set<char> delimiters) const;

    bool skip_token(const char &token,
            const std::unordered_set<char> delimiters);
    bool skip_token(const std::string &token,
            const std::unordered_set<char> delimiters);

    bool end_reached() const;

};

void Buffer::increment(const unsigned int steps = 1) {
    index += steps;
}

void Buffer::skip_characters(const std::unordered_set<char> &characters) {
    while((characters.find(text[index]) != characters.end()) && index < length)
        index += 1;
}

Buffer::Buffer(const std::string &text) {
    this->text = text;
    this->length = text.length();

    line = 0;
    index = 0;
}

std::string Buffer::get_token(const std::unordered_set<char> delimiters =
        Buffer::default_delimiters) const {

    std::string token;
    unsigned int offset = index;
    while((delimiters.find(text[offset]) == delimiters.end()) &&
            offset < length) {

        token += text[offset];
        offset += 1;
    }

    return token;
}

std::string Buffer::skip_token(const std::unordered_set<char> delimiters =
        Buffer::default_delimiters) {

    std::string token;
    while((delimiters.find(text[index]) == delimiters.end()) &&
        index < length) {

        token += text[index];
        index += 1;
    }
    skip_characters(delimiters);

    return token;
}

bool Buffer::get_token(const char &token) const {

    return ((index < length) && (text[index] == token));
}

bool Buffer::get_token(const std::string &token) const {

    return text.substr(index, token.length()) == token;
}

bool Buffer::skip_token(const char &token,
        const std::unordered_set<char> delimiters =
        Buffer::default_delimiters) {

    
}

bool Buffer::skip_token(const std::string &token,
        const std::unordered_set<char> delimiters =
        Buffer::default_delimiters) {

}

bool Buffer::end_reached() const {}
    return index >= length;
};



// ******************************************************************* Operation

class Operation {

    static std::shared_ptr<Operation> parse(Parse::Buffer &buffer);

};

class Transient : public Operation {

public:

    double start_time;
    double stop_time;
    double time_step;

    static std::shared_ptr<Transient> parse(Parse::Buffer);

};

std::shared_ptr<Transient> Transient::parse(Parse::Buffer &buffer) {
    std::vector<std::reference_wrapper<double>> parameters;
    for(auto &parameter : parameters) {

    }
}

std::shared_ptr<Operation> parse(Parse::Buffer &buffer) {
    if(buffer.get_token(".tran"))
        return Transient::parse(buffer);
    else
        return nullptr;
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
        throw Schematic::NullComponentPointerException();

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

    passive->name = buffer.skip_token();

    passive->nodes[0] = buffer.skip_token();
    passive->nodes[1] = buffer.skip_token();

    try {
        passive->value = Parse::metric_value(buffer.skip_token());
    }
    catch(const Parse::exception &exception) {
        return false;
    }

    if(buffer.skip_token('\n') == false)
        return false;

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
    void simulate(Operation &operation, const Schematic &schematic, const double &time);

};

Capacitor::Capacitor() : Component(Component::Type::CAPACITOR) {}

bool Capacitor::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Passive::parse<Capacitor>(buffer, schematic, 'C');
}

template <Transient transient>
void Capacitor::simulate(Transient &transient, const Schematic &schematic, const double &time);

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
void Inductor::simulate(Transient &transient, const Schematic &schematic, const double &time);

class Resistor : public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Resistor();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic, const double &time);

};

bool Resistor::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Passive::parse<Resistor>(buffer, schematic, 'R');
}

Resistor::Resistor() : Component(Component::Type::RESISTOR) {}

template <Transient transient>
void Resistor::simulate(Transient &transient, const Schematic &schematic, const double &time);

// *********************************************************** Source base class

class Function {

public:

    virtual double value(const double &time) const = 0;

};

class Constant : public Function {

public:

    double value(const double &time) const override;

};

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

    source->name = buffer.skip_token();

    source->nodes[0] = buffer.skip_token();
    source->nodes[1] = buffer.skip_token();

    const auto function = Function::parse(buffer);
    if(function == nullptr)
        return false;
    else
        source->function = function;

    if(buffer.skip_token('\n') == false)
        return false;

    schematic.add_component(source);
}

double Source::value(const double &time) const {
    if(function == nullptr)
        throw Source::MissingFunctionException();
    else
        return function->value(time);
}

// *********************************************************** Source components

class CurrentSource : public Source, public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    CurrentSource();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic, const double &time);

};

bool CurrentSource::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Source::parse<CurrentSource>(buffer, schematic, 'I');
}

CurrentSource::CurrentSource() : Component(Component::Type::CURRENT_SOURCE) {}

template <Transient transient>
void CurrentSource::simulate(Transient &transient, const Schematic &schematic, const double &time);

class VoltageSource : public Source, public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    VoltageSource();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic, const double &time);

};

bool VoltageSource::parse(Parse::Buffer &buffer, Schematic &schematic) {
    return Source::parse<VoltageSource>(buffer, schematic, 'V');
}

VoltageSource::VoltageSource() : Component(Component::Type::VOLTAGE_SOURCE) {}

template <Transient transient>
void VoltageSource::simulate(Transient &transient, const Schematic &schematic, const double &time);

// ****************************************************************** Semiconductors

class Diode : public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Diode();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic, const double &time);

};

bool Diode::parse(Parse::Buffer &buffer, Schematic &schematic) {
    std::shared_ptr<Diode> diode(new Diode());

    diode->name = buffer.skip_token();

    diode->nodes[0] = buffer.skip_token();
    diode->nodes[1] = buffer.skip_token();

    if(buffer.skip_token('D') == false)
        return false;

    if(buffer.skip_token('\n') == false)
        return false;

    schematic.add_component(diode);
    return true;
}

Diode::Diode() : Component(Component::Type::DIODE) {}

template <Transient transient>
void Diode::simulate(Transient &transient, const Schematic &schematic, const double &time);

class Transistor : public Component {

    static bool parse(Parse::Buffer &buffer, Schematic &schematic);

    Transistor();

    template <typename Operation>
    void simulate(Operation &operation, const Schematic &schematic, const double &time);

};

bool Transistor::parse(Parse::Buffer &buffer, Schematic &schematic) {
    auto transistor = std::shared_ptr<Transistor>(new Transistor());

    transistor->name = buffer.skip_token();

    transistor->nodes[0] = buffer.skip_token();
    transistor->nodes[1] = buffer.skip_token();
    transistor->nodes[2] = buffer.skip_token();

    if(buffer.skip_token("NPN"))
        transistor->model = Transistor::Model::NPN;
    else if(buffer.skip_token("PNP"))
        transistor->model = Transistor::Model::PNP;
    else
        return false;

    if(buffer.skip_token('\n') == false)
        return false;

    schematic.add_component(transistor);
    return true;
}

Transistor::Transistor() : Component(Component::Type::TRANSISTOR) {}

template <Transient transient>
void Transistor::simulate(Transient &transient, const Schematic &schematic, const double &time);

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
    else if(buffer.get_token('D'))
        return Diode::parse(buffer, schematic);
    else if(buffer.get_token('Q'))
        return Transistor::parse(buffer, schematic);
    else
        return false;
}

int main(int argument_count, char *argument_vector[]) {}

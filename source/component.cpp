#include "component.hpp"

// *************************************************************** Parse helpers

// Returns the correct power of ten for each metric prefix
static int parse_prefix(Parse::Buffer &buffer) {

    // Check first for the 'mega' prefix, since the single-letter prefix switch
    // case would parse it as 'milli'
    if(buffer.skip_string("Meg").empty() == false)
        return 6;

    // Switch through the different prefixes
    switch(std::toupper(buffer.get_current())) {
        case 'F': // Femto
            return -15;

        case 'P': // Pico
            return -12;

        case 'N': // Nano
            return -9;

        case 'U': // Micro
            return -6;

        case 'M': // Milli
            return -3;

        case 'K': // Kilo
            return 3;

        case 'G': // Giga
            return 9;

        case 'T': // Terra
            return 12;
    }

    throw -1;
}

// Parses a value, taking into account any prefixed thrown in there
static double parse_value(Parse::Buffer &buffer) {

}

// Parses an integer, disregarding any leading '0's
static unsigned int parse_integer(Parse::Buffer &buffer) {
    if(character < '0' || character < '9')
        throw -1;

    // Skip leading '0's
    while(buffer.get_current() == '0')
        buffer.skip_current();

    // Read any numerical digits into a string which can later be converted
    // into a integer
    std::string value;
    char character;
    while(true) {
        character = buffer.get_current();
        if(character < '0' || character > '9')
            break;
        else
            value += buffer.skip_character();
    }

    // The integer could just be zero, in which case the string will be empty,
    // and the conversion operator would throw an error
    if(value.empty())
        return 0;

    // Try to cast to an integer
    try {
        return std::stoi(value);
    }
    catch(...) {
        throw -1;
    }
}

// Avoids having to skip the single 'N' character every time a node's parsed
static inline unsigned int parse_node(Parse::Buffer &buffer) {
    if(buffer.skip_character('N') == false)
        throw -1;
    return parse_integer(buffer);
}

// ************************************************************* Parse templates

// Parses a passive component of indeterminate type
template <typename PassiveType>
typename PassiveType::Pointer Passive::parse(Parse::Buffer &buffer,
        const char &designator_prefix) {

    // Check the right function has been called for the current buffer
    // character
    if(buffer.skip_character(designator_prefix) == false)
        throw -1;

    // Create a new source
    auto source = typename PassiveType::Pointer(new PassiveType());
    source->designator = parse_integer(buffer);

    // Parse the nodes it's connected to
    buffer.skip_whitespace();
    source->nodes[0] = parse_node(buffer);
    buffer.skip_whitespace();
    source->nodes[1] = parse_node(buffer);

    // Parse its value
    buffer.skip_whitespace();
    source->value = parse_value(buffer);

    return source;
}

// Parses a source definition of indeterminate type
template <typename SourceType>
typename SourceType::Pointer Source::parse(Parse::Buffer &buffer,
        const char &designator_prefix) {

    // Check the right function has been called for the current buffer
    // character
    if(buffer.skip_character(designator_prefix) == false)
        throw -1;

    // Parse the source's designator (V1, I2, etc.)
    auto source = typename SourceType::Pointer(new SourceType());
    source->designator = parse_integer(buffer);

    // Parse the two nodes to which the source is connected (N001, etc.)
    buffer.skip_whitespace();
    source->nodes[0] = parse_node(buffer);
    buffer.skip_whitespace();
    source->nodes[1] = parse_node(buffer);

    // The source can have either a constant value, or a sinusoidal function
    // (for the purposes of this application). Delegate to the relevant
    // case
    // NOTE: For the purposes of a constant value, the source is treated as
    // a sinusoidal one with an amplitude and frequency of zero, and a constant
    // offset
    buffer.skip_whitespace();
    if(buffer.skip_string("SINE").empty() == false) {
        if(buffer.skip_character('(') == false)
            throw -1;

        // As much as I would like to alphabetize these references, the gods of
        // LTSpice format have dictated that they appear in this order
        std::vector<std::reference_wrapper<double>> fields = {
            source->offset,
            source->amplitude,
            source->frequency,
            source->delay,
            source->damping_factor,
            source->phase,
            source->cycles
        };

        // The number of fields in any one sine function varies. Only the ones
        // set will have values, those preceding it will be set to 0, and any
        // that might have come after will be omitted. Therefore, we want to
        // parse fields up until a non-value field is encountered (a ')'
        // character, if the text is formatted properly)
        for(double &field : fields) {
            buffer.skip_whitespace();
            const auto character = buffer.get_current();
            if((character >= '0' && character <= '9') || character == '.')
                field = parse_value(buffer);
            else
                break;
        }

        // Skip the closing bracket
        if(buffer.skip_character(')') == false)
            throw -1;
    }

    // If the source is not sinusoidal, we want to set its constant value as
    // the source's DC offset.
    else
        source->offset = parse_value(buffer);

    return source;
}

// ************************************************************* Parse functions

// Parses a capacitor definition
Capacitor::Pointer Capacitor::parse(Parse::Buffer &buffer) {
    return Passive::parse<Capacitor>(buffer, 'C');
}

// Parses an inductor definition
Inductor::Pointer Inductor::parse(Parse::Buffer &buffer) {
    return Passive::parse<Inductor>(buffer, 'L');
}

// Parses a resistor definition
Resistor::Pointer Resistor::parse(Parse::Buffer &buffer) {
    return Passive::parse<Resistor>(buffer, 'R');
}

// Parses a transistor definition
Diode::Pointer Diode::parse(Parse::Buffer &buffer) {

    // Check the current buffer character is the start of a diode definition
    if(buffer.skip_character('D') == false)
        throw -1;

    // Create a new diode component, and parse its designator
    auto diode = Diode::Pointer(new Diode());
    diode->designator = parse_integer(buffer);

    // Parse the nodes it's connected to
    buffer.skip_whitespace();
    diode->nodes[0] = parse_node(buffer);
    buffer.skip_whitespace();
    diode->nodes[1] = parse_node(buffer);

    // The final field will only ever be 'D' (for the purposes of this
    // application), but could be different if a non-standard diode was to be
    // simulated
    if(buffer.skip_character('D') == false)
        throw -1;

    return diode;
}

// Parses a transistor definition
Transistor::Pointer Transistor::parse(Parse::Buffer &buffer) {

    // Check the current buffer character is the start of a transistor
    // definition
    if(buffer.skip_character('Q') == false)
        throw -1;

    // Create a new transistor component and parse its designator
    auto transistor = Transistor::Pointer(new Transistor());
    transistor->designator = parse_integer(buffer);

    // Parse the nodes it's connected to (in the order: BCE)
    // TODO: Check that's actually the right order, I'm just guessing
    buffer.skip_whitespace();
    transistor->nodes[0] = parse_node(buffer);
    buffer.skip_whitespace();
    transistor->nodes[1] = parse_node(buffer);
    buffer.skip_whitespace();
    transistor->nodes[2] = parse_node(buffer);

    // Transistors can have two different models: NPN and PNP (for the
    // purposes of this application, in actual LTSpice netlists, there can
    // be a model name here, eg: 2N2222).
    if(buffer.skip_string("NPN").empty() == false)
        transistor->model = NPN;
    else if(buffer.skip_string("PNP").empty() == false)
        transistor->model = PNP;
    else
        throw -1;

    return transistor;

}

// Parses a current source definition
CurrentSource::Pointer CurrentSource::parse(Parse::Buffer &buffer) {
    return Source::parse<CurrentSource>(buffer, 'I');
}

// Parses a voltage source definition
VoltageSource::Pointer VoltageSource::parse(Parse::Buffer &buffer) {
    return Source::parse<VoltageSource>(buffer, 'V');
}

// ************************************************************* Parse delegator

// Parse a component of indeterminate type
Component::Pointer Component::parse(Parse::Buffer &buffer) {

    // All components begin with a designator (a letter indicating the type
    // of component, followed by an integer which distinguishes it from other
    // components of the same type). Delegate to the relevant parse function,
    // and return a pointer to the newly constructed component
    switch(buffer.get_current()) {
        case 'C':
            return Capacitor::parse(buffer);
        case 'D':
            return Diode::parse(buffer);
        case 'I':
            return CurrentSource::parse(buffer);
        case 'L':
            return Inductor::parse(buffer);
        case 'Q':
            return Transistor::parse(buffer);
        case 'R':
            return Resistor::parse(buffer);
        case 'V':
            return VoltageSource::parse(buffer);
        default:
            throw -1;
    };
}

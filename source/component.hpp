#pragma once

// Returns true if the character is any number 0-9
static bool is_integer(const char &character) {
    return character >= '0' && character <= '9';
}

// Returns an integer at the current buffer position
static unsigned int parse_integer(ParseBuffer &buffer) {
    std::string value;
    while(is_integer(buffer.get_current()))
        value += buffer.skip_current();

    if(value.empty())
        throw -1;

    try {
        return std::stoi(value);
    }
    catch(...) {
        throw -1;
    }
}

// Returns a multiplier value (the power of 10 which the suffix raises it to)
static int parse_multiplier(ParseBuffer &buffer) {
    switch(buffer.get_current()) {
        case 'p':
            return -12;
        case 'n':
            return -9;
        case 'u':
            return -6;
        case 'm':
            return -3;
        case 'k':
            return 3;
        case 'G':
            return 9;
    }

    if(buffer.skip_string("Meg").empty() == false)
        return 6;

    throw -1;
}

// Returns a value parsed from the current buffer position
static double parse_value(ParseBuffer &buffer) {

}

// Parses a node name in the format "Nxxx", where xxx is an integer
static unsigned int parse_node(ParseBuffer &buffer) {
    if(buffer.skip_character('N') == false)
        throw -1;

    return parse_integer(buffer);
}

static double parse_double(ParseBuffer &buffer) {

}

CurrentSource::Pointer CurrentSource::parse(ParseBuffer &buffer);
VoltageSource::Pointer VoltageSource::parse(ParseBuffer &buffer);

Diode::Pointer Diode::parse(ParseBuffer &buffer);
Transistor::Pointer Transistor::parse(ParseBuffer &buffer);

Capacitor::Pointer Capacitor::parse(ParseBuffer &buffer);
Inductor::Pointer Inductor::parse(ParseBuffer &buffer);
Resistor::Pointer Resistor::parse(ParseBuffer &buffer);

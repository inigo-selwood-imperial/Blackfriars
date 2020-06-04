#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include <cmath>

/* ******************************************************************** Synopsis

The Parse namespace encapsulates all the functions needed to parse a text file.
It's been co-opted from my JSON parse library, and adapted to remove some of the
redundant functionality. LTSpice netlist files are, luckily, super-simple.

The Buffer class is used to wrap the logic of iterating through a string
simpler. The buffer contains an index, which is moved around in the context of
the text which the buffer wraps. Individual characters and strings can be read
or skipped, relative to the position of the index within the text. There are
also functions for conditional skipping, which will return a character/string
if a matching one is provided.

The position sub-struct is used to move between set points in the buffer. Say
a given parse function branches in two directions depending on information which
is only available at a later time, a position struct can be extracted at the
start, and then potentially rewound to at a later time -- without having to use
the more computationally expensive decrement function.

The source file is split into 9 parts:

    (1) Definition

    (2) Print operator
        This makes printing the position of an unexpected character/string
        easier from the perspective of parse functions

    (3) Increment/decrement operators
        These functions allow the index position to be moved around within the
        text. This also updates internal members like the line and column
        number -- although the position information functions which these
        supported have been removed in the current context.

    (4) Constructors

    (5) Get functions
        These functions allow characters/strings to be accessed at the current
        index, without skipping past them. It can be more computationally
        expensive to use these functions instead of skip functions, so use them
        sparingly? I suppose?

    (6) End reached test

    (7) Skip functions
        I'm quite proud of myself for coming up with this one, they're
        especially helpful with parsing markup languages like JSON and YAML.
        Basically, you provide a character/string, and the Buffer class will
        return that text if it's present within the text at the current index.
        This way, the function will return 'false', if the character/text isn't
        present.

    (8) Parse helpers
        These functions make it easier to extract fields from the text, such as
        integers, values in scientific notation, metric values, etc.

    (9) Position helpers
        These functions allow you to change the index's position within the
        buffer, or get a position struct with information about this current
        position. This position class has an std::ostream print operator, which
        neatly serializes the line and column data

*/

// ****************************************************************** Definition

namespace Parse {

std::string load_file(const std::string &file_name);

enum Whitespace {
    COMMENTS,
    NEWLINES,
    SPACES,
    TABS
};

class Buffer {

private:

    std::string _text;

    std::vector<unsigned int> _line_start_indices;

    unsigned int _index;
    unsigned int _length;

    unsigned int _line;
    unsigned int _column;

    void increment(const unsigned int steps);
    void decrement(const unsigned int steps);

public:

    struct Position {

        unsigned int index;

        unsigned int line;
        unsigned int column;

        Position(const unsigned int &index, const unsigned int &line,
                const unsigned int &column);

    };

    Buffer(const std::string &text);

    char get_current() const;
    char get_next() const;
    bool get_string(const std::string &text) const;

    bool end_reached() const;

    char skip_current();
    char skip_character(const char &character);
    void skip_whitespace(const int flags);
    bool skip_string(const std::string &text);

    Buffer::Position Buffer::get_position() const;
    void Buffer::set_position(const Position &position);

};

// ************************************************************** Print operator

std::ostream &operator(std::ostream &stream, const Buffer::Position &position) {
    return stream << "line " << position.line << " column " << position.column;
}

// *********************************************** Increment/decrement operators

// Moves the index forward in the text, updating line/column information
void Buffer::increment(const unsigned int steps = 1) {
    for(unsigned int offset = 0; offset < steps; offset += 1) {
        _index += 1;
        if(_text[_index] == '\n') {
            _line += 1;
            _column = 0;
        }
        else
            _column += 1;
    }
}

// Moves the index backward in the text, updating line/column information
void Buffer::decrement(const unsigned int steps = 1) {
    for(unsigned int offset = 0; offset < steps; offset += 1) {
        _index -= 1;
        if(_text[_index] == '\n') {
            _line -= 1;
            if(_line) {
                _column = _line_start_indices[_line + 1] -
                        _line_start_indices[_line];
            }
        }
        else
            _column -= 1;
    }
}

// **************************************************************** Constructors

Buffer::Buffer(const std::string &text) {
    if(text.empty())
        return;
    _text = text;

    _length = text.length();

    _line_start_indices.push_back(0);
    for(unsigned int offset = 0; offset < _length; offset += 1) {
        if(_text[offset] == '\n' && (offset + 1) < _length &&
                _text[offset + 1] != '\n') {

            _line_start_indices.push_back(_index + 1);
        }
    }

    unsigned int line_index = 0;
    for(const auto &start_offset : _line_start_indices) {
        _line_start_indices.push_back(0);
        for(unsigned int offset = start_offset; offset < _length; offset += 1) {

            if(_text[offset] == '\t')
                _line_start_indices[line_index] += 4;
            else if(_text[offset] == ' ')
                _line_start_indices[line_index] += 1;
            else
                break;
        }

        line_index += 1;
    }

    _index = 0;
    _line = 0;
    _column = 0;
}

Buffer::Position::Position(const unsigned int &index, const unsigned int &line,
        const unsigned int &column) {

    this->index = index;
    this->line = line;
    this->column = column;
}

// *************************************************************** Get functions

// Gets the current character in the text
char Buffer::get_current() const {
    return _index < _length ? _text[_index] : 0;
}

// Gets the next charcter in the text
char Buffer::get_next() const {
    return ((_index + 1) < _length) ? _text[_index + 1] : 0;
}

// Returns true if the string provided is encountered at the current position
// in the text
bool Buffer::get_string(const std::string &text) const {
    return _index < _length ? _text[_index] : 0;
}

// ************************************************************ End reached test

// True if the index has reached the end of the text
bool Buffer::end_reached() const {
    return _index >= _length;
}

// ************************************************************** Skip functions

// Returns the character at the current index, skipping past it
char Buffer::skip_current() {
    auto value = get_current();
    increment();
    return value;
}

// Skips the current character in the text, if it matches a given character
char Buffer::skip_character(const char &character) {
    if(get_current() == character)
        return skip_current();
    return 0;
}

// Skips whitespace, returning the characters skipped as a string, optionally
// specifying the types of whitespace to skip (comments, newlines, etc.)
void Buffer::skip_whitespace(const int flags = 0xF) {
    while(true) {
        if(end_reached())
            break;

        else if((get_current() == ' ' && (flags & Whitespace::SPACES)) ||
                (get_current() == '\t' && (flags & Whitespace::TABS)) ||
                (get_current() == '\n' && (flags & Whitespace::NEWLINES)))
            skip_current();

        else if(get_current() == '*') {
            while(true) {
                if(get_current() == '\n' || end_reached())
                    break;
                skip_current();
            }
        }

        else
            break;
    }
}

// Skip a given string, returning it if it's encountered at the current position
// in the text,
bool Buffer::skip_string(const std::string &text) {
    if(_index + text.length() >= _length)
        return "";

    else if(text == _text.substr(_index, text.length())) {
        increment(text.length());
        return true;
    }

    return false;
}

/* *************************************************************** Parse helpers

The parse helpers are designed to help classes using the Parse::Buffer class to
isolate specialized fields from the text: natural numbers, integers, etc.

The parse helpers have some re-usable static functions, used for identifying
the whether a character might form part of a specialized field, for
extracting those fields, and lastly, the externally visible functions for
'parsing' those fields from the buffer

*/

// True if the character is an integer (0-9 or a '-' symbol)
static bool is_integer(const char &character) {
    return (character >= '0' && character <= '9') || character == '-';
}

static const std::unordered_set<char> metric_prefixes = {'f', 'p', 'n', 'u',
        'm', 'k', 'M', 'g', 't'};

// True if the character is a metric prefix symbol (micro 'u', nano 'n', etc.)
// NOTE: This isn't 100% reliable in the case of the "Meg" prefix
static bool is_metric_symbol(const char &character) {
    return metric_prefixes.find(character) != metric_prefixes.end();
}

// True if the character is 0-9
static bool is_natural_number(const char &character) {
    return (character >= '0' && character <= '9');
}

// True if the character is a number 0-9, '-', or '.'
static bool is_number(const char &character) {
    return (character >= '0' && character <= '9') || character == '-' ||
            character == '.';
}

// Returns a power of 10 represented by a metric prefix symbol
static inline int parse_metric_symbol(Buffer &buffer) {

    if(buffer.skip_string("Meg"))
        return 6;

    switch(buffer.skip_current()) {
        case 'f':
            return -15;
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
        case 'g':
            return 9;
        case 't':
            return 12;
    }

    std::cerr << "Couldn't recognize metric symbol" << std::endl;
    throw -1;
}

// Returns an integer parsed from the buffer
int integer(Buffer &buffer) {
    std::string value;
    while(is_integer(buffer.get_current()))
        value += buffer.skip_current();

    if(value.empty()) {
        std::cerr << "Expected integer" << std::endl;
        throw -1;
    }

    return std::stoi(value);
}

// Returns a natural number extracted from the buffer
unsigned int natural_number(Buffer &buffer) {
    std::string value;
    while(is_natural_number(buffer.get_current()))
        value += buffer.skip_current();

    if(value.empty()) {
        std::cerr << "Expected natural number" << std::endl;
        throw -1;
    }

    return std::stoi(value);
}

// Returns a number extracted from the buffer
double number(Buffer &buffer) {
    std::string value;
    while(is_number(buffer.get_current()))
        value += buffer.skip_current();

    if(value.empty()) {
        std::cerr << "Expected number" << std::endl;
        throw -1;
    }

    return std::stof(value);
}

// Parse a metric value, as used in electrical component values
double metric_value(Buffer &buffer) {
    std::string string_value;

    while(is_number(buffer.get_current()))
        string_value += buffer.skip_current();

    auto factor = 0;
    if(is_metric_symbol(buffer.get_current()))
        factor = parse_metric_symbol(buffer);

    if(is_number(buffer.get_current()) && factor)
        string_value += ".";

    while(is_number(buffer.get_current()))
        string_value += buffer.skip_current();

    return std::stof(string_value) * std::pow(10, factor);
}

// ************************************************************ Position helpers

// Gets the buffer's position
// Position data includes the column, line, and current index
Buffer::Position Buffer::get_position() const {
    return Position(_index, _line + 1, _column + 1);
}

// Sets the buffer's position
// If the line/column data doesn't match the index, an error is thrown
void Buffer::set_position(const Position &position) {
    if(_index != (_line_start_indices[_line] + _column)) {
        std::cerr << "Couldn't set position within parse buffer, since the "
                "index data didn't match internal data" << std::endl;
        throw -1;
    }

    _index = position.index;
    _line = position.line;
    _column = position.column;
}

}; // Namespace Parse

#pragma once

#include <string>
#include <vector>

// Flags for skipping whitespace
enum Whitespace {
    COMMENTS    = 1,
    NEWLINES    = 1 << 1,
    SPACES      = 1 << 2,
    TABS        = 1 << 3
};

// Allows iteration through a block of text, with helpers for getting data
// about line, column, and indentation
class ParseBuffer {

private:

    std::string text;

    std::vector<unsigned int> line_start_indices;

    unsigned int index;
    unsigned int length;

    unsigned int line;
    unsigned int column;

    void increment(const unsigned int &steps);
    void decrement(const unsigned int &steps);

public:

    struct Position {

        unsigned int index;

        unsigned int line;
        unsigned int column;

        Position(const unsigned int &index, const unsigned int &line,
                const unsigned int &column) {

            this->index = index;
            this->line = line;
            this->column = column;
        }

    };

    ParseBuffer(const std::string &text);

    char get_current() const;
    char get_next() const;

    bool end_reached() const;

    char skip_current();
    char skip_character(const char &character);
    std::string skip_whitespace(const int &flags);
    std::string skip_string(const std::string &text);

    Position get_position() const;
    void set_position(const Position &position);

    int indentation_delta(int line) const;

};

// Increments the buffer's position within the text, updating line and column
// information accordingly
void ParseBuffer::increment(const unsigned int &steps = 1) {
    for(unsigned int offset = 0; offset < steps; offset += 1) {
        index += 1;
        if(text[index] == '\n') {
            line += 1;
            column = 0;
        }
        else
            column += 1;
    }
}

// Decrements the buffer's position within the text, updating line and column
// information accordingly
void ParseBuffer::decrement(const unsigned int &steps = 1) {
    for(unsigned int offset = 0; offset < steps; offset += 1) {
        index -= 1;
        if(text[index] == '\n') {
            line -= 1;
            if(line) {
                column = line_start_indices[line + 1] -
                        line_start_indices[line];
            }
        }
        else
            column -= 1;
    }
}

ParseBuffer::ParseBuffer(const std::string &text) {
    if(text.empty())
        return;
    this->text = text;

    line_start_indices.push_back(0);
    for(unsigned int index = 0; index < text.length(); index += 1) {
        if(text[index] == '\n' && (index + 1) < text.length() &&
                text[index + 1] != '\n') {

            line_start_indices.push_back(index + 1);
        }
    }

    unsigned int line = 0;
    for(const auto &start_index : line_start_indices) {
        line_start_indices.push_back(0);
        for(unsigned int index = start_index; index < text.length();
                index += 1) {

            if(text[index] == '\t')
                line_start_indices[line] += 4;
            else if(text[index] == ' ')
                line_start_indices[line] += 1;
            else
                break;
        }

        line += 1;
    }

    this->index = 0;
    this->line = 0;
    this->column = 0;
}

// Gets the current character in the text, or 0 if the end is reached
char ParseBuffer::get_current() const {
    return index < length ? text[index] : 0;
}

// Gets the next character in the text, or 0 if the end is reached
char ParseBuffer::get_next() const {
    return ((index + 1) < length) ? text[index + 1] : 0;
}

// True if the index has reached the end of the text
bool ParseBuffer::end_reached() const {
    return index >= length;
}

// Skips the current character, returning its value (or 0 if the end has been
// reached)
char ParseBuffer::skip_current() {
    auto value = get_current();
    increment();
    return value;
}

// Skips a character, returning it if encountered, or 0 otherwise
char ParseBuffer::skip_character(const char &character) {
    if(get_current() == character)
        return skip_current();
    return 0;
}

// Skips specified types of whitespace, returning the whitespace encountered
std::string ParseBuffer::skip_whitespace(const int &flags = 0xF) {
    std::string result;
    while(true) {
        if(end_reached())
            break;

        else if((get_current() == ' ' && (flags & SPACES)) ||
                (get_current() == '\t' && (flags & TABS)) ||
                (get_current() == '\n' && (flags & NEWLINES)))
            result += skip_current();

        else if(get_current() == '#') {
            while(true) {
                if(get_current() == '\n' || end_reached())
                    break;
                result += skip_current();
            }
        }

        else
            break;
    }

    return result;
}

// Skips a string, returning it if found
std::string ParseBuffer::skip_string(const std::string &text) {
    if(index + text.length() >= length)
        return "";

    else if(text == this->text.substr(index, text.length())) {
        increment(text.length());
        return text;
    }

    return "";
}

// Gets the buffer's position
// Position data includes the column, line, and current index
ParseBuffer::Position ParseBuffer::get_position() const {
    return Position(index, line + 1, column + 1);
}

// Sets the buffer's position
// If the line/column data doesn't match the index, an error is thrown
void ParseBuffer::set_position(const Position &position) {
    if(index != (line_start_indices[line] + column))
        throw -1;

    index = position.index;
    line = position.line;
    column = position.column;
}

#include "parse.hpp"

namespace Parse {

// ************************************************************ File load helper

// More memory and computationally efficient than constructing a stringstream
// object, according to StackOverflow
std::string load_file(const std::string &name) {
    std::ifstream stream(name, std::ios::in);
    if(stream) {
        std::ostringstream contents;
        contents << stream.rdbuf();
        stream.close();
        return contents.str();
    }

    // If the stream wasn't valid, throw an error
    std::cerr << "Couldn't open file " << name << std::endl;
    throw -1;
}

// ********************************************************************** Buffer

// Increments the buffer's position within the text, updating line and column
// information accordingly
void Buffer::increment(const unsigned int &steps = 1) {
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
void Buffer::decrement(const unsigned int &steps = 1) {
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

Buffer::Buffer(const std::string &text) {
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
char Buffer::get_current() const {
    return index < length ? text[index] : 0;
}

// Gets the next character in the text, or 0 if the end is reached
char Buffer::get_next() const {
    return ((index + 1) < length) ? text[index + 1] : 0;
}

// True if the index has reached the end of the text
bool Buffer::end_reached() const {
    return index >= length;
}

// Skips the current character, returning its value (or 0 if the end has been
// reached)
char Buffer::skip_current() {
    auto value = get_current();
    increment();
    return value;
}

// Skips a character, returning it if encountered, or 0 otherwise
char Buffer::skip_character(const char &character) {
    if(get_current() == character)
        return skip_current();
    return 0;
}

// Skips specified types of whitespace, returning the whitespace encountered
std::string Buffer::skip_whitespace(const int &flags = 0xF) {
    std::string result;
    while(true) {
        if(end_reached())
            break;

        else if((get_current() == ' ' && (flags & SPACES)) ||
                (get_current() == '\t' && (flags & TABS)) ||
                (get_current() == '\n' && (flags & NEWLINES)))
            result += skip_current();

        else if(get_current() == '*') {
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
std::string Buffer::skip_string(const std::string &text) {
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
Buffer::Position Buffer::get_position() const {
    return Position(index, line + 1, column + 1);
}

// Sets the buffer's position
// If the line/column data doesn't match the index, an error is thrown
void Buffer::set_position(const Position &position) {
    if(index != (line_start_indices[line] + column)) {
        std::cerr << "Couldn't set position within parse buffer, since the "
                "index data didn't match internal data" << std::endl;
        throw -1;
    }

    index = position.index;
    line = position.line;
    column = position.column;
}

}; // Namespace Parse

#pragma once

#include <fstream> // std::ifstream
#include <iostream> // std::cerr
#include <sstream> // std::ostringstream
#include <string>
#include <vector>

namespace Parse {

std::string load_file(const std::string &name);

// Flags for skipping whitespace
enum Whitespace {
    COMMENTS    = 1,
    NEWLINES    = 1 << 1,
    SPACES      = 1 << 2,
    TABS        = 1 << 3
};

// Allows iteration through a block of text, with helpers for getting data
// about line, column, and indentation
class Buffer {

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

    Buffer(const std::string &text);

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
}; // Namespace Parse

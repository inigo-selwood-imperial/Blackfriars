#pragma once

namespace Parse {

class Buffer {

private:

    std::string text;

    unsigned int length;

    unsigned int index;
    unsigned int line;

    void increment(const unsigned int steps);

    void skip_characters(const std::unordered_set<char> &characters);

public:

    Buffer(const std::string &text);

    bool end_reached() const ;

};

// Moves forward the index by a given number of steps
void Buffer::increment(const unsigned int steps = 1) {
    index += steps;
}

// Increments the index until a character *not* in the characters set is
// reached, or the end of the file is encountered
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

// Returns true if the index has reached the end of the buffer
bool Buffer::end_reached() const {
    return index >= length;
}

};

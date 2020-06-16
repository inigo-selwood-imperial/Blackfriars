#pragma once

#include <string>
#include <unordered_set>

class TextBuffer {

private:

    std::string text;

    unsigned int index;
    unsigned int length;
    unsigned int line_number;

    void increment(const unsigned int steps);

public:

    TextBuffer &operator=(const std::string &text);

    TextBuffer();
    TextBuffer(const std::string &text);

    bool end_reached() const;
    unsigned int get_line_number() const;

    char get_character(const bool skip);
    std::string get_string(const bool skip,
            const std::unordered_set<char> terminators);

    bool skip_character(const char &character);
    void skip_characters(const std::unordered_set<char> &characters);
    void skip_line();
    bool skip_string(const std::string &string);
    void skip_whitespace(const std::unordered_set<char> characters);

};

// Increment the index by a given number of steps (1 step by default)
void TextBuffer::increment(const unsigned int steps = 1) {
    for(unsigned int offset = 0; offset < steps; offset += 1) {
        if(index >= length)
            return;

        index += 1;
        if(text[index] == '\n')
            line_number += 1;
    }
}

// Assign a block of text to the buffer
TextBuffer &TextBuffer::operator=(const std::string &text) {
    this->text = text;
    length = text.length();

    index = 0;
    line_number = 1;

    return *this;
}

TextBuffer::TextBuffer() {}

TextBuffer::TextBuffer(const std::string &text) {
    *this = text;
}

// Returns true if the index has reached the end of the text
bool TextBuffer::end_reached() const {
    return index >= length;
}

// Returns the number of the current line
unsigned int TextBuffer::get_line_number() const {
    return line_number;
}

// Returns the number of the current line within the text, or 0 if the end of
// the text has been reached
// If 'skip' is true, then the index will be incremented to skip past this
// character
char TextBuffer::get_character(const bool skip = false) {
    if(index < length) {
        const auto &character = text[index];
        if(skip)
            increment();
        return character;
    }

    return 0;
}

// Returns a string of characters, up until a terminator character (or the end
// of the text) is reached.
// If 'skip' is true, then the index will be incremented to skip past the string
std::string TextBuffer::get_string(const bool skip = false,
        const std::unordered_set<char> terminators = {' ', '\t', '\n'}) {

    auto offset = index;
    std::string result;
    while(offset < length && terminators.find(text[offset]) ==
            terminators.end()) {

        result += text[offset];
        offset += 1;
    }

    if(skip) {
        for(auto delta = index; delta < offset; delta += 1)
            increment();
    }

    return result;
}

// Returns true if the character provided is encountered at the current index
// within the text, and increments the index to skip past it. If the character
// isn't encountered, the index remains unaffected
bool TextBuffer::skip_character(const char &character) {
    if(index >= length)
        return false;

    if(text[index] != character)
        return false;

    increment();
    return true;
}

// Skips all characters encountered in the set
void TextBuffer::skip_characters(const std::unordered_set<char> &characters) {
    while(end_reached() == false && characters.find(get_character()) !=
            characters.end()) {

        increment();
    }
}

// Skips an entire line (up until the next newline character, or the end of the
// text)
void TextBuffer::skip_line() {
    while(end_reached() == false && get_character() != '\n')
        increment();
}

// Returns true if the string provided is encountered at the current index
// within the text, and increments the index to skip past it. If the string
// isn't encountered, the index remains unaffected
bool TextBuffer::skip_string(const std::string &string) {
    if(text.substr(index, string.length()) != string)
        return false;

    increment(string.length());
    return true;
}

// Skips whitespace characters (by default: spaces and tabs)
void TextBuffer::skip_whitespace(const std::unordered_set<char> characters =
        {' ', '\t'}) {

    return skip_characters(characters);
}

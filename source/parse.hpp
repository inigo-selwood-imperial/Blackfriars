#pragma once

#include <iostream>
#include <locale>
#include <map>
#include <string>

#include <cmath>

// Parses a metric value
// FIXME: Doesn't handle all weird decimal point placements
double parse_metric_value(const std::string &value) {

    // Map containing the metric symbols and their corresponding factors
    static std::map<char, int> metric_prefixes = {
        {'f', -15},
        {'p', -12},
        {'n', -9},
        {'u', -6},
        {'m', -3},
        {'k', 3},
        {'g', 9},
        {'t', 12}
    };

    // Split the 'value' string into a 'result' string containing only a number,
    // and a 'factor' variable containing any metric symbol's value
    std::string result;
    int factor = 0;
    for(unsigned int index = 0; index < value.length(); index += 1) {
        const auto character = value[index];

        // Add numbers and formatting characters straight to the 'result'
        // string
        if(character >= '0' && character <= '9' || character == '-' ||
                character == '.') {

            result += character;
        }

        // If the character isn't a number, it must be part of a metric symbol,
        // so try and find out which one, setting the 'factor' variable
        else {

            // Make sure we aren't overwriting anything
            if(factor) {
                std::cerr << "Encountered multiple metric factors in a single "
                        "value: '" << value << "'" << std::endl;
                throw -1;
            }

            // The 'mega' and 'mu' prefixes are a string and a multi-character
            // constant respectively, so they can't be placed in the map of
            // symbols
            else if(value.substr(index, 3) == "Meg") {
                factor = 6;
                index += 3;
            }
            else if(value.substr(index, 2) == std::string({-62, -75})) {
                factor = -6;
                index += 2;
            }

            // Find the right factor from the symbol map
            else if(metric_prefixes.find(std::tolower(character)) !=
                    metric_prefixes.end()) {

                factor = metric_prefixes[character];
            }

            // If none of the above have been the case, then the character
            // encountered wasn't part of a metric symbol
            else {
                std::cerr << "Couldn't parse metric value '" << value << "'" <<
                        std::endl;
                throw -1;
            }

            // If the metric symbol was encountered part-way through the string,
            // as with, for instance, "4k7", then it should be replaced by a
            // decimal point
            if(index < value.length())
                result += '.';
        }
    }

    return std::stof(result) * std::pow(10, factor);
}

// Parse a time stamp
double parse_time_value(const std::string &value) {
    std::string result = value;
    if(result.back() == 's')
        result.pop_back();
    return parse_metric_value(result);
}

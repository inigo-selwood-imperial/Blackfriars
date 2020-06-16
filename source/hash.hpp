#pragma once

#include <functional>

typedef unsigned int Hash;

Hash hash_value(const std::string &string) {
    return std::hash<std::string>{}(string);
}

Hash hash_node(const std::string &node) {
    return (node == "0") ? 0 : hash_value(node);
}

Hash hash_combine(Hash one, const Hash &two) {
    return one ^= two + 0x9e3779b9 + (one << 6) + (one >> 2);
}

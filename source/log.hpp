#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <ctime>

namespace Log {

static std::string timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream stream;
    stream << std::put_time(std::localtime(&in_time_t), "[%d.%m.%Y %H:%M:%S]");
    return stream.str();
}

std::ostream &alert() {
    return std::cout << timestamp() << " ALERT: ";
}

std::ostream &debug() {
    return std::cout << timestamp() << " DEBUG: ";
}

std::ostream &error() {
    return std::cerr << timestamp() << " ERROR: ";
}

}; // Namespace log

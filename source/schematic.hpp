#pragma once

#include <vector>
#include <string>

#include "component.hpp"
#include "parse.hpp"

class Schematic {

public:

    Schematic(const std::string &specification) {
        parse_specification(specification);
    }

private:

    std::vector<Component::Pointer> _components;

    friend std::ostream &operator<<(std::ostream &stream,
            const Schematic &schematic) {

        for(const auto &component : schematic._components) {
            switch(component->type) {
                case Component::CAPACITOR:
                    stream << "capacitor" << std::endl;
                    break;
                case Component::RESISTOR:
                    stream << "resistor" << std::endl;
                    break;
                case Component::INDUCTOR:
                    stream << "inductor" << std::endl;
                    break;
            }
        }

        return stream;
    }

    void parse_specification(const std::string &specification) {
        Parse::Buffer buffer(specification);

        _components.clear();
        while(true) {
            buffer.skip_whitespace();
            if(buffer.end_reached())
                break;

            _components.push_back(Component::parse(buffer));
        }
    }

};

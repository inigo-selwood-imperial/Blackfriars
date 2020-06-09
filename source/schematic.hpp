#pragma once

class Schematic {

private:

    std::vector<std::shared_ptr<Component>> components;

    std::map<Component::Type, unsigned int> component_counts;

public:

    unsigned int get_component_count(const Component::Type &type) {
        return component_counts[type];
    }


};

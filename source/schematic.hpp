#pragma once

#include <map>
#include <memory>
#include <vector>
#include <iterator>

#include "components/templates/component.hpp"

class Schematic {

private:

    std::vector<std::shared_ptr<Component>> components;

    std::map<Component::Type, std::vector<std::shared_ptr<Component>>>
            component_types;

    unsigned int node_count;

public:

    void add_component(const std::shared_ptr<Component> &component);

    std::vector<std::shared_ptr<Component>> get_components();

    bool empty() const;

};

// Adds a component to the schematic
void Schematic::add_component(const std::shared_ptr<Component> &component) {
    if(component == nullptr)
        return;

    components.push_back(component);
}

// Return the components
std::vector<std::shared_ptr<Component>> Schematic::get_components() {
    return components;
}

// Returns true if there are no components in the schematic
bool Schematic::empty() const {
    return components.empty();
}

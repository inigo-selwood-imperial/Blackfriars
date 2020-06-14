#pragma once

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <vector>

#include "components/templates/component.hpp"

class Schematic {

private:

    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::string> node_names;

    std::map<Component::Type, std::vector<std::shared_ptr<Component>>>
            component_types;

    unsigned int node_count;

public:

    void add_component(const std::shared_ptr<Component> &component);

    std::vector<std::shared_ptr<Component>> get_components();
    std::vector<std::string> get_node_names();

    bool empty() const;

};

// Adds a component to the schematic
void Schematic::add_component(const std::shared_ptr<Component> &component) {
    if(component == nullptr)
        return;

    components.push_back(component);
    for(const auto &node_name : component->nodes) {
        if(node_name == "0")
            continue;
        else if(std::find(node_names.begin(), node_names.end(), node_name) ==
                node_names.end()) {

            node_names.push_back(node_name);
        }
    }
}

// Return the components
std::vector<std::shared_ptr<Component>> Schematic::get_components() {
    return components;
}

std::vector<std::string> Schematic::get_node_names() {
    return node_names;
}

// Returns true if there are no components in the schematic
bool Schematic::empty() const {
    return components.empty();
}

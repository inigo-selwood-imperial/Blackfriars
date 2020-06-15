#pragma once

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <vector>

#include "components/templates/component.hpp"
#include "hash.hpp"

class Schematic {

private:

    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::string> node_names;
    std::vector<Hash> node_hashes;

    std::map<Component::Type, std::vector<std::shared_ptr<Component>>>
            component_types;

    unsigned int node_count;

public:

    void add_component(const std::shared_ptr<Component> &component);

    std::vector<std::shared_ptr<Component>> get_components(const int types);
    std::vector<std::string> get_node_names();
    std::vector<Hash> get_node_hashes();

    bool empty() const;

};

// Adds a component to the schematic
void Schematic::add_component(const std::shared_ptr<Component> &component) {
    if(component == nullptr)
        return;

    components.push_back(component);
    for(const auto &node_name : component->node_names) {
        if(node_name == "0")
            continue;
        else if(std::find(node_names.begin(), node_names.end(), node_name) ==
                node_names.end()) {

            node_names.push_back(node_name);
        }
    }
    for(const auto &node_hash : component->node_hashes) {
        if(node_hash == 0)
            continue;
        else if(std::find(node_hashes.begin(), node_hashes.end(), node_hash) ==
                node_hashes.end()) {

            node_hashes.push_back(node_hash);
        }
    }
    component_types[component->type].push_back(component);
}

// Return the components
std::vector<std::shared_ptr<Component>> Schematic::get_components(
        const int types = 0) {

    if(types == 0)
        return components;

    std::vector<std::shared_ptr<Component>> result;
    for(const auto &pair : component_types) {
        if(types & pair.first) {
            result.insert(result.end(), pair.second.begin(),
                    pair.second.end());
        }
    }
    return result;
}

std::vector<std::string> Schematic::get_node_names() {
    return node_names;
}

std::vector<Hash> Schematic::get_node_hashes() {
    return node_hashes;
}

// Returns true if there are no components in the schematic
bool Schematic::empty() const {
    return components.empty();
}

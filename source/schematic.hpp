#pragma once

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <vector>

#include "components/templates/component.hpp"
#include "utilities/hash.hpp"

class Schematic {

private:

    std::vector<std::pair<std::string, Hash>> nodes;

    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::pair<Component::Type, Hash>> component_hashes;
    std::vector<std::string> node_names;
    std::vector<Hash> node_hashes;

    std::map<Component::Type, std::vector<std::shared_ptr<Component>>>
            component_types;

    unsigned int node_count;

public:

    void add_component(const std::shared_ptr<Component> &component);

    std::vector<std::pair<std::string, Hash>> get_nodes() {
        return nodes;
    }

    std::vector<std::string> get_node_names();
    std::vector<Hash> get_node_hashes();
    std::vector<std::shared_ptr<Component>> get_components(const int types);
    std::vector<std::pair<Component::Type, Hash>> get_component_hashes();

    bool empty() const;

};

std::vector<std::pair<Component::Type, Hash>>
        Schematic::get_component_hashes() {

    return component_hashes;
}

// Adds a component to the schematic
void Schematic::add_component(const std::shared_ptr<Component> &component) {
    if(component == nullptr)
        return;

    components.push_back(component);
    for(unsigned int index = 0; index < component->node_names.size(); index += 1) {
        const std::string name = component->node_names[index];
        Hash hash = component->node_hashes[index];
        if(name == "0")
            hash = 0;

        const std::pair<std::string, Hash> node_signature = {name, hash};
        if(std::find(nodes.begin(), nodes.end(), node_signature) == nodes.end())
            nodes.push_back(node_signature);
    }

    component_hashes.push_back({component->type, component->hash});
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

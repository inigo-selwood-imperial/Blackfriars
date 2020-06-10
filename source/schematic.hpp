#pragma once

class Schematic {

public:

    std::vector<std::shared_ptr<Component>> components;

    void add_component(const std::shared_ptr<Component> &component);

};

// TODO: Flesh out a bit
void Schematic::add_component(const std::shared_ptr<Component> &component) {
    if(component == nullptr)
        throw -1;

    components.push_back(component);
}

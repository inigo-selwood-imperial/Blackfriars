#pragma once

class Schematic {

private:

    std::vector<std::shared_ptr<Component>> components;

public:

    void add_component(const std::shared_ptr<Component> &component);

};

void Schematic::add_component(const std::shared_ptr<Component> &component) {
    if(component == nullptr)
        return;

    components.push_back(component);
}

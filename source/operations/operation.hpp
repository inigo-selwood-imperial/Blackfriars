#pragma once

class Schematic;

class Operation {

public:

    virtual bool run(Schematic &schematic,
            std::shared_ptr<std::ostream> stream) = 0;

};

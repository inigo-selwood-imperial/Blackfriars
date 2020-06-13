#pragma once

class Schematic;

class Operation {

public:

    virtual bool run(Schematic &schematic,
            const std::ostream &stream) = 0;

};

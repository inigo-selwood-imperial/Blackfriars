#pragma once

class Schematic;

class Operation {

public:

    virtual bool run(Schematic &schematic,
            std::ostream &stream) = 0;

};

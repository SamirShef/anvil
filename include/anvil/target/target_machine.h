#pragma once
#include "anvil/target/data_layout.h"
#include "anvil/target/triple.h"

namespace anvil {

class Module;
class MachineModule;

class TargetMachine {
protected:
    Triple     _triple;
    DataLayout _dl;

public:
    TargetMachine (const TargetMachine &) = delete;
    TargetMachine (TargetMachine &&)      = default;
    TargetMachine &
    operator= (const TargetMachine &) = delete;
    TargetMachine &
    operator= (TargetMachine &&) = default;

    TargetMachine (Triple triple, DataLayout dl) : _triple (triple), _dl (dl) {}

    virtual ~TargetMachine () = default;

    virtual void
    SelectInstructions (Module &inMod, MachineModule &outMod) = 0;

    const Triple &
    GetTriple () const {
        return _triple;
    }

    const DataLayout &
    GetDataLayout () const {
        return _dl;
    }
};

}
